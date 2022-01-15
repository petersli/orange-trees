#ifndef CANVAS2D_H
#define CANVAS2D_H

#include <memory>

#include "SupportCanvas2D.h"

class RayScene;

class CS123SceneCameraData;

/**
 * @class Canvas2D
 */
class Canvas2D : public SupportCanvas2D {
    Q_OBJECT
public:
    Canvas2D();
    virtual ~Canvas2D();

    void setScene(RayScene *scene);

    // UI will call this from the button on the "Ray" dock
    void renderImage(CS123SceneCameraData *camera, int width, int height);

    // This will be called when the settings have changed
    virtual void settingsChanged();

    QImage* getImage() { return m_image; }

    // Set the color of a pixel in the canvas data
    void setPixelColor(RGBA* pixels, int row, int col, RGBA color);


public slots:
    // UI will call this from the button on the "Ray" dock
    void cancelRender();

protected:
    virtual void paintEvent(QPaintEvent *);  // Overridden from SupportCanvas2D.
    virtual void mouseDown(int x, int y);    // Called when left mouse button is pressed on canvas
    virtual void mouseDragged(int x, int y); // Called when left mouse button is dragged on canvas
    virtual void mouseUp(int x, int y);      // Called when left mouse button is released

    // Called when the size of the canvas has been changed
    virtual void notifySizeChanged(int w, int h);



private:
    // Ray
    std::unique_ptr<RayScene> m_rayScene;
};

#endif // CANVAS2D_H
