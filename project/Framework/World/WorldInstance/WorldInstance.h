#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

class WorldInstance: public World {
    private:
        Camera* camera;

        Mirror mirror;
        MinimapContainer minimap;
        InfiniteTerrain infinite_terrain;

    protected:

        // method called only once
        void Start() {
            Reporter::println("Start method called");
            camera = getCamera();

            camera->translate(vec3(0.0f, 0.5f, 0.0f));
            camera->scale(vec3(-0.2f, -0.2f, -0.2f));

            setupMirror();
            setupMinimap();
            setupInfiniteTerrain();
        }

        // method called every frame
        void Update() {
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
            minimap.update();
            infinite_terrain.update();
        }

        void setupMirror() {
            Camera* mirror_camera = instantiate(new Camera());
            enableLiveRenderer(mirror_camera);
            mirror.setMirrorCamera(mirror_camera);
            mirror.setTargetCamera(camera);
            mirror.setup();
        }

        void setupMinimap() {
            Camera* viewer_camera = instantiate(new Camera());
            enableLiveRenderer(viewer_camera);
            minimap.setBackground(instantiate2D(new Minimap()));
            minimap.setViewer(viewer_camera, instantiate2D(new LiveViewer(viewer_camera->getRenderTextureID())));
            minimap.setTargetCamera(camera);
            minimap.setup();
        }

        void setupInfiniteTerrain() {
            GLuint mirror_texture_id = mirror.getMirrorTextureID();
            infinite_terrain.setTarget(camera);
            for (int i = 0; i < 4; i++) {
                infinite_terrain.setChunk(i, Chunk(instantiate(new Terrain()), instantiate(new Water(mirror_texture_id))));
            }
            infinite_terrain.initialize();
        }
};