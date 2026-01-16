#include "mediaplayer.h"
#include "ui_mediaplayer.h"
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QTime>
#include <QFileInfo>

MediaPlayer::MediaPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaPlayer),
    layout_video(nullptr),
    player(nullptr),
    widget(nullptr),
    timer(nullptr),
    maxValue(1000),
    play_state(false),
    if_reload(false),
    state_slider_volume(false)
{
    ui->setupUi(this);

    // 设置视频显示区域的背景色
    ui->label->setStyleSheet("background-color: black; border: 1px solid gray;");

    // 音量滑块
    slider_Volume = new CustomSlider(this);
    slider_Volume->setOrientation(Qt::Vertical);
    slider_Volume->setRange(0, 100);
    slider_Volume->setValue(50);

    // 初始化按钮状态
    ui->pushButton_Player->setEnabled(false);
    ui->pushButton_Volume->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider->setRange(0, maxValue);
    slider_Volume->setEnabled(false);
    slider_Volume->hide();

    // 连接信号与槽
    connect(ui->horizontalSlider, &CustomSlider::costomSliderClicked,
            this, &MediaPlayer::horizontalSlider_clicked);
    connect(ui->horizontalSlider, &CustomSlider::sliderMoved,
            this, &MediaPlayer::horizontalSlider_moved);
    connect(ui->horizontalSlider, &CustomSlider::sliderReleased,
            this, &MediaPlayer::horizontalSlider_released);
    connect(slider_Volume, &CustomSlider::costomSliderClicked,
            this, &MediaPlayer::slider_Volume_Changed);
    connect(slider_Volume, &CustomSlider::sliderMoved,
            this, &MediaPlayer::slider_Volume_Changed);
}

MediaPlayer::~MediaPlayer()
{
    cleanupMediaPlayer();
    delete slider_Volume;
    delete ui;
}

void MediaPlayer::cleanupMediaPlayer()
{
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    if (player) {
        player->stop();
        delete player;
        player = nullptr;
    }

    if (widget) {
        delete widget;
        widget = nullptr;
    }

    if (layout_video) {
        // 清理布局前先移除控件
        layout_video->removeWidget(widget);
        delete layout_video;
        layout_video = nullptr;
    }
}

void MediaPlayer::initMediaPlayer()
{
    cleanupMediaPlayer();

    player = new QMediaPlayer(this);
    widget = new QVideoWidget(this);
    layout_video = new QVBoxLayout;

    player->setVideoOutput(widget);

    // 连接错误信号
    connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &MediaPlayer::handlePlayerError);

    // 连接媒体状态变化信号
    connect(player, &QMediaPlayer::mediaStatusChanged,
            this, &MediaPlayer::handleMediaStatusChanged);

    // 设置布局
    layout_video->setContentsMargins(0, 0, 0, 0);
    layout_video->setSpacing(0);
    widget->setMinimumSize(400, 300);
    layout_video->addWidget(widget);

    // 清除 label 的原有布局
    if (ui->label->layout()) {
        delete ui->label->layout();
    }
    ui->label->setLayout(layout_video);

    // 创建定时器
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, &MediaPlayer::onTimerOut);
}

bool MediaPlayer::isValidVideoFile(const QString& filePath)
{
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.exists())
        return false;

    // 检查文件扩展名
    QStringList supportedFormats = {".avi", ".mp4", ".mkv", ".mov", ".wmv", ".flv"};
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    return !suffix.isEmpty() && supportedFormats.contains("." + suffix);
}

void MediaPlayer::handlePlayerError(QMediaPlayer::Error error)
{
    qDebug() << "Player error:" << error << player->errorString();

    // 使用 canonicalUrl() 替代被移除的 request() 方法
    QString mediaUrl = player->media().canonicalUrl().toString();
    if (mediaUrl.isEmpty()) {
        mediaUrl = "Unknown file";
    }

    QMessageBox::warning(this, "播放错误",
                         QString("无法播放视频:\n%1\n\n错误信息: %2")
                         .arg(mediaUrl)
                         .arg(player->errorString()));

    ui->pushButton_Player->setEnabled(false);
    ui->pushButton_Volume->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
}

void MediaPlayer::handleMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    qDebug() << "Media status changed:" << status;
    if (status == QMediaPlayer::LoadedMedia) {
        qDebug() << "Media loaded. Duration:" << player->duration() << "ms";
    } else if (status == QMediaPlayer::BufferedMedia) {
        qDebug() << "Media buffered";
    } else if (status == QMediaPlayer::InvalidMedia) {
        qDebug() << "Invalid media";
    }
}

void MediaPlayer::on_pushButton_Open_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("选择视频文件"),
        QDir::homePath(),
        tr("视频格式(*.avi *.mp4 *.flv *.mkv *.mov *.wmv);;所有文件(*.*)"));

    if (filename.isEmpty())
        return;

    if (!isValidVideoFile(filename)) {
        QMessageBox::warning(this, "错误", "选择的文件无效或格式不支持");
        return;
    }

    initMediaPlayer();

    QUrl fileUrl = QUrl::fromLocalFile(filename);
    player->setMedia(fileUrl);

    // 设置初始音量
    player->setVolume(50);
    slider_Volume->setValue(50);

    // 启用控制按钮
    ui->pushButton_Volume->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    ui->pushButton_Player->setEnabled(true);

    if_reload = true;

    // 开始播放
    on_pushButton_Player_clicked();
}

void MediaPlayer::on_pushButton_Player_clicked()
{
    if (!player) return;

    if (player->state() == QMediaPlayer::PlayingState) {
        player->pause();
        ui->pushButton_Player->setText("播放");
        if (timer) timer->stop();
    } else {
        player->play();
        ui->pushButton_Player->setText("暂停");
        if (timer) timer->start();
    }
}

void MediaPlayer::on_pushButton_Volume_clicked()
{
    if (!player) return;

    if (state_slider_volume) {
        slider_Volume->setEnabled(false);
        slider_Volume->hide();
    } else {
        slider_Volume->setEnabled(true);
        slider_Volume->setValue(player->volume());

        // 计算音量滑块位置
        int x = ui->pushButton_Volume->x() +
                ui->pushButton_Volume->width() / 2 - 15;
        int y = ui->pushButton_Volume->y() - 100;
        slider_Volume->setGeometry(x, y, 30, 102);
        slider_Volume->show();
    }
    state_slider_volume = !state_slider_volume;
}

void MediaPlayer::onTimerOut()
{
    if (!player || player->duration() <= 0) return;

    qint64 position = player->position();
    qint64 duration = player->duration();

    if (duration > 0) {
        int value = static_cast<int>((position * maxValue) / duration);
        ui->horizontalSlider->setValue(value);

        // 更新 label_time 显示
               QTime currentTime(0, 0, 0);
               currentTime = currentTime.addMSecs(position);
               QTime totalTime(0, 0, 0);
               totalTime = totalTime.addMSecs(duration);

               ui->label_time->setText(
                   currentTime.toString("mm:ss") + " / " +
                   totalTime.toString("mm:ss")
               );
    }
}

void MediaPlayer::slider_Volume_Changed()
{
    if (player) {
        player->setVolume(slider_Volume->value());
    }
}

void MediaPlayer::horizontalSlider_clicked()
{
    if (!player || player->duration() <= 0) return;

    int value = ui->horizontalSlider->value();
    qint64 newPosition = static_cast<qint64>((value * player->duration()) / maxValue);
    player->setPosition(newPosition);
}

void MediaPlayer::horizontalSlider_moved()
{
    if (timer) timer->stop();
    horizontalSlider_clicked();
}

void MediaPlayer::horizontalSlider_released()
{
    if (player && player->state() == QMediaPlayer::PlayingState && timer) {
        timer->start();
    }
}
