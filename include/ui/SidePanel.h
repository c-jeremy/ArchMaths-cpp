#pragma once

#include <cstring>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <vector>
#include "math/MathTypes.h"

namespace ArchMaths {

class EntryWidget;

class SidePanel : public QWidget {
    Q_OBJECT

public:
    explicit SidePanel(QWidget* parent = nullptr);

    void addEntry(const PlotEntry& entry);
    void updateEntry(int index, const PlotEntry& entry);
    void removeEntry(int index);
    void clear();

signals:
    void entryChanged(int index, const QString& expression);
    void entryDeleted(int index);
    void entryVisibilityChanged(int index, bool visible);
    void entryColorChanged(int index, const Color& color);
    void entryParameterChanged(int index, const QString& name, double value);
    void addEntryRequested();
    void precisionChanged(double multiplier);

private:
    void setupUI();

    QVBoxLayout* entriesLayout_;
    QScrollArea* scrollArea_;
    QPushButton* addButton_;
    QSlider* precisionSlider_;
    QLabel* precisionLabel_;
    std::vector<EntryWidget*> entryWidgets_;
};

} // namespace ArchMaths
