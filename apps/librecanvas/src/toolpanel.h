#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <memory>
#include "tool.h"

class ToolPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ToolPanel(QWidget *parent = nullptr);
    void setTool(std::shared_ptr<LibreCanvas::Tool> tool);

signals:
    void toolChanged(std::shared_ptr<LibreCanvas::Tool> tool);
    void brushSizeChanged(int size);
    void brushHardnessChanged(float hardness);
    void brushOpacityChanged(float opacity);
    void brushColorChanged(const QColor& color);

private slots:
    void onToolButtonClicked(int id);
    void onBrushSizeChanged(int value);
    void onBrushHardnessChanged(int value);
    void onBrushOpacityChanged(int value);
    void onColorButtonClicked();

private:
    void setupUI();
    void updateToolProperties();

    QButtonGroup *m_toolGroup;
    QToolButton *m_brushBtn;
    QToolButton *m_eraserBtn;
    QToolButton *m_marqueeBtn;
    QToolButton *m_magicWandBtn;
    
    QSlider *m_sizeSlider;
    QSlider *m_hardnessSlider;
    QSlider *m_opacitySlider;
    QPushButton *m_colorBtn;
    QLabel *m_sizeLabel;
    QLabel *m_hardnessLabel;
    QLabel *m_opacityLabel;
    
    std::shared_ptr<LibreCanvas::Tool> m_currentTool;
    QColor m_brushColor;
};

