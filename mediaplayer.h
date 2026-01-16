#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMainWindow>
#include "customslider.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVBoxLayout>

namespace Ui {
class MediaPlayer;
}

class MediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

private slots:
    void on_pushButton_Volume_clicked();
    void on_pushButton_Open_clicked();
    void on_pushButton_Player_clicked();

    //自定义槽函数
    void horizontalSlider_clicked();
    void horizontalSlider_moved();
    void horizontalSlider_released();
    void slider_Volume_Changed();
    void onTimerOut();
    void handlePlayerError(QMediaPlayer::Error error);
    void handleMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    Ui::MediaPlayer *ui;
    CustomSlider *slider_Volume;

    QVBoxLayout* layout_video;
    QMediaPlayer* player;
    QVideoWidget* widget;
    QTimer *timer;
    int maxValue;

    bool play_state;
    bool if_reload;
    bool state_slider_volume;

    void initMediaPlayer();
    void cleanupMediaPlayer();
    bool isValidVideoFile(const QString& filePath);
};

#endif // MEDIAPLAYER_H
