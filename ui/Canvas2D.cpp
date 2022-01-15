/**
 * @file Canvas2D.cpp
 *
 * CS123 2-dimensional canvas, contains support code.
 *
 */

// For your convenience, a few headers are included for you.
#include <assert.h>
#include <iostream>
#include <math.h>
#include <memory>
#include <unistd.h>
#include "Canvas2D.h"
#include "Settings.h"
#include "RayScene.h"

#include <QCoreApplication>
#include <QPainter>

Canvas2D::Canvas2D() :
    m_rayScene(nullptr)
{
}

Canvas2D::~Canvas2D()
{
}

// This is called when the canvas size is changed. You can change the canvas size by calling
// resize(...). You probably won't need to fill this in, but you can if you want to.
void Canvas2D::notifySizeChanged(int w, int h) {
}

void Canvas2D::paintEvent(QPaintEvent *e) {
    // You probably won't need to fill this in, but you can if you want to override any painting
    // events for the 2D canvas. For now, we simply call the superclass.
    SupportCanvas2D::paintEvent(e);

}
void Canvas2D::settingsChanged() {
    std::cout << "Canvas2d::settingsChanged() called. Settings have changed" << std::endl;
}

void Canvas2D::mouseDown(int x, int y) {
}

void Canvas2D::mouseDragged(int x, int y) {
}

void Canvas2D::mouseUp(int x, int y) {
}

// ********************************************************************************************
// ** RAY
// ********************************************************************************************

void Canvas2D::setScene(RayScene *scene) {
    m_rayScene.reset(scene);
}

void Canvas2D::renderImage(CS123SceneCameraData *camera, int width, int height) {
    if (m_rayScene) {
        m_rayScene->renderRayScene(this, camera, width, height);
    }
}

void Canvas2D::cancelRender() {
    if (m_rayScene) {
        m_rayScene->stopRendering();
    }
}

/**
 *  Set pixel at row, col to the provided color.
 *  Note: you still need to call canvas->update() if you use this function
 */
void Canvas2D::setPixelColor(RGBA* pixels, int row, int col, RGBA color) {
    int index = row * width() + col;
    pixels[index] = color;
}




