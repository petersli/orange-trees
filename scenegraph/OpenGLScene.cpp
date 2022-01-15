#include "OpenGLScene.h"

#include <GL/glew.h>

#include "Settings.h"

OpenGLScene::~OpenGLScene()
{
}

void OpenGLScene::setClearColor() {
    if (settings.drawWireframe || settings.drawNormals) {
        glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    } else {
        glClearColor(135.0f/255, 206.0f/255, 235.0f/255, 0.8f);
    }
}

void OpenGLScene::settingsChanged() {

}
