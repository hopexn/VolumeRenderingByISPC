#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

#include <boost/timer.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;
using namespace boost;

#include "ispc/Renderer.h"
#include "Vector.hpp"
#include "Camera.hpp"

class DisplayWidget : public QWidget {
Q_OBJECT
public:
    DisplayWidget(int _width, int _height) {
        width = _width;
        height = _height;
        pixels = new float[width * height];
        qimage = new QImage(width, height, QImage::Format_ARGB32);

        int N = dims[0] * dims[1] * dims[2];
        auto buf = new unsigned char[N];
        data = new int[N];

        fstream fin("../data/aneurism.raw", ios::in | ios::binary);
        if (!fin.is_open()) {
            cout << "Can't open volume file!" << endl;
            exit(1);
        }
        fin.read(reinterpret_cast<char *>(buf), N);
        fin.close();
        for (int i = 0; i < N; i++) {
            data[i] = buf[i];
        }
        delete[] buf;
    }

    ~DisplayWidget() {
        delete[] pixels;
        delete[] data;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        timer t;
        t.restart();

        float cam[4][3];
        Vector3f cam_pos, cam_dir, cam_up, cam_right;
        cam_pos = camera.getPosition();
        cam_dir = camera.getLookDirection();
        cam_up = camera.getUpDirection();
        cam_right = camera.getRightDirection();

        cam[0][0] = cam_pos.x;
        cam[0][1] = cam_pos.y;
        cam[0][2] = cam_pos.z;

        cam[1][0] = cam_dir.x;
        cam[1][1] = cam_dir.y;
        cam[1][2] = cam_dir.z;

        cam[2][0] = cam_up.x;
        cam[2][1] = cam_up.y;
        cam[2][2] = cam_up.z;

        cam[3][0] = cam_right.x;
        cam[3][1] = cam_right.y;
        cam[3][2] = cam_right.z;

        float cam_plane_dist = camera.getPlaneDistance();

        float bbox[3][3] = {{-0.5f, -0.5f, -0.5f},
                            {0.5f,  0.5f,  0.5f},
                            {1,     1,     1}};

        ispc::renderImage(cam, cam_plane_dist, width, height, dims, bbox, data, pixels);

        cout << "Time cost: " << t.elapsed() << "s" << endl;

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int index = j * width + i;
                int gray = (int) pixels[index];
                qimage->setPixel(i, j, qRgb(gray, gray, gray));
            }
        }

        painter.drawImage(QRect(0, 0, width, height), *qimage);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        float dx = float(event->x() - lastPosition.x()) / width;
        float dy = float(event->y() - lastPosition.y()) / height;
        if (event->buttons() & Qt::RightButton) {
            camera.translate(dx, -dy);
        } else if (event->buttons() & Qt::LeftButton) {
            camera.rotate(dx, dy);
        }
        lastPosition = event->pos();
        update();
    }

/**
 * 滚轮事件， 控制放缩
 * @param event
 */
    void wheelEvent(QWheelEvent *event) override {
        if (event->delta() > 0) {
            camera.scale(0.88f);
        } else {
            camera.scale(1 / 0.88f);
        }
        update();
    }

/*
 * 鼠标事件，当鼠标按下后记录鼠标位置
 */
    void mousePressEvent(QMouseEvent *event) override {
        lastPosition = event->pos();
    }

private:
    int width, height;
    float *pixels;

    QImage *qimage;
    QPoint lastPosition;
    Camera camera;

    int dims[3] = {256, 256, 256};
    int *data;
};
