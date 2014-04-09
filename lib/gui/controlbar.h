#ifndef CONTROLBAR_H
#define CONTROLBAR_H

#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include "../optionsmanager/optionsman.h"
#include "../timemanager/timeManager.h"

namespace visualizer
{

class ControlBar : public QWidget
{
  friend class _GUI;
  friend class _TimeManager;
  Q_OBJECT
    public:
    ControlBar( QWidget *parent = 0 );

  public slots:
    void sliderPress();
    void sliderRelease();
    void sliderChanged(int value);
    void rewind();
    void play();
    void fastForward();
    void turnChanged();
    void maxTurnsChanged(int);

  private:
    QLabel* turnLabel;
    QSlider* m_slider;
    QPushButton* rewindButton;
    QPushButton* playButton;
    QPushButton* fastForwardButton;

};

} //visualizer

#endif
