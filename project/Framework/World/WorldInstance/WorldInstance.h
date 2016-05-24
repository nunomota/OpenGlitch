#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

class WorldInstance: public World {
    private:
        Camera* camera;

        Mirror mirror;
        Mirror refraction;
        MinimapContainer minimap;

        Terrain* terrain;
        Water* water;
        InfiniteTerrain infinite_terrain;
        Sky* sky;

        LiveViewer* reflection_texture;
        LiveViewer* refraction_texture;

    protected:

        // method called only once
        void Start() {
            Reporter::println("Start method called");
            camera = getCamera();
            getLight()->rotate(vec3(-45.0f, 0.0f, 0.0f));


            camera->translate(vec3(0.0f, 0.5f, 0.0f));
            
            sky = instantiate(new Sky());
            sky->rotate(vec3(180.0f,0.0f,0.0f));
            sky->scale(vec3(40.0f,40.0f,40.0f));
            sky->getTransform()->setPosition(camera->getTransform()->getPosition());

            sky = instantiate(new Sky());
            sky->rotate(vec3(180.0f,0.0f,0.0f));
            sky->scale(vec3(40.0f,40.0f,40.0f));
            sky->getTransform()->setPosition(camera->getTransform()->getPosition());

            camera->translate(vec3(0.0f, 1.0f, 0.0f));
            camera->scale(vec3(-0.2f, -0.2f, -0.2f));
            camera->rotate(vec3(-45.0f, 0.0f, 0.0f));

            setupMirror();
            setupRefraction();
            setupMinimap();
            //setupInfiniteTerrain();

            terrain = instantiate(new Terrain(getTime(), getLight(), getCamera()));
            water = instantiate(new Water(mirror.getMirrorTextureID(), refraction.getMirrorTextureID(), getTime(), getLight(), getCamera()));

            camera->getTransform()->setPosition(terrain->getTransform()->getPosition());
            camera->translate(vec3(0.0f, 2.0f, 0.0f));
            camera->getTransform()->setRotation(vec3(-90.0f, 0.0f, 0.0f));

            reflection_texture = instantiate2D(new LiveViewer(mirror.getMirrorTextureID()));
            reflection_texture->rotate(vec3(90.0f, 0.0f, 0.0f));
            reflection_texture->translate(vec3(0.75f, 0.75f, 0.0f));
            reflection_texture->scale(vec3(-0.79f, 0.0f, -0.79f));

            refraction_texture = instantiate2D(new LiveViewer(refraction.getMirrorTextureID()));
            refraction_texture->rotate(vec3(90.0f, 0.0f, 0.0f));
            refraction_texture->translate(vec3(0.75f, 0.0f, 0.0f));
            refraction_texture->scale(vec3(-0.79f, 0.0f, -0.79f));
        }

        // method called every frame
        void Update() {
            // sideways camera turn
            if (getKeyDown(Keyboard::W)) {
                getCamera()->rotate(vec3(-90.0f, 0.0f, 0.0f) * getTime()->getDeltaTime());
            } else if (getKeyDown(Keyboard::S)) {
                getCamera()->rotate(vec3(90.0f, 0.0f, 0.0f) * getTime()->getDeltaTime());
            }

            // sideways camera turn
            if (getKeyDown(Keyboard::D)) {
                getCamera()->rotate(vec3(0.0f, -90.0f, 0.0f) * getTime()->getDeltaTime());
            } else if (getKeyDown(Keyboard::A)) {
                getCamera()->rotate(vec3(0.0f, 90.0f, 0.0f) * getTime()->getDeltaTime());
            }

            // front/back camera movement
            if (getKeyDown(Keyboard::P)) {
                getCamera()->translate(getCamera()->getTransform()->getForwardVector() * getTime()->getDeltaTime());
            } else if (getKeyDown(Keyboard::L)) {
                getCamera()->translate(-getCamera()->getTransform()->getForwardVector() * getTime()->getDeltaTime());
            }


            mirror.update();
            refraction.update();
            // update sky box
            sky->getTransform()->setPosition(camera->getTransform()->getPosition());

            minimap.update();
            //infinite_terrain.update();

            vec3 camera_position = camera->getTransform()->getPosition();
            terrain->getTransform()->setPosition(vec3(camera_position.x, 0.0f, camera_position.z));
            water->getTransform()->setPosition(vec3(camera_position.x, 0.0f, camera_position.z));
        }

        void setupMirror() {
            Camera* mirror_camera = instantiate(new Camera());
            enableLiveRenderer(mirror_camera);
            mirror.setMirrorCamera(mirror_camera);
            mirror.setTargetCamera(camera);
            mirror.setClipPlane(vec4(0.0f, 1.0f, 0.0f, 0.0f));
            mirror.setup();
        }

        void setupRefraction() {
            Camera* refraction_camera = instantiate(new Camera());
            enableLiveRenderer(refraction_camera);
            refraction.setMirrorCamera(refraction_camera);
            refraction.setTargetCamera(mirror.getMirrorCamera());
            refraction.setClipPlane(vec4(0.0f, -1.0f, 0.0f, 0.0f));
            refraction.setup();
        }

        void setupMinimap() {
            Camera* viewer_camera = instantiate(new Camera());
            enableLiveRenderer(viewer_camera);
            minimap.setBackground(instantiate2D(new Minimap()));
            minimap.setViewer(viewer_camera, instantiate2D(new LiveViewer(viewer_camera->getRenderTextureID())));
            minimap.setTargetCamera(camera);
            minimap.setup();
        }
        /*
        void setupInfiniteTerrain() {
            GLuint mirror_texture_id = mirror.getMirrorTextureID();
            infinite_terrain.setTarget(camera);
            for (int i = 0; i < 4; i++) {
                infinite_terrain.setChunk(i, Chunk(instantiate(new Terrain()), instantiate(new Water(mirror_texture_id))));
            }
            infinite_terrain.initialize();
        }*/
};