#ifndef ARRERA_THEME_CPP
#define ARRERA_THEME_CPP

#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QPalette>
#include <QStyleHints>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>

class Arrera_Theme : public QObject
{
    Q_OBJECT

public:
    explicit Arrera_Theme(QObject *parent = nullptr);
    bool loadThemeFromJson(const QString &filePath);
    void applyTheme(Qt::ColorScheme scheme);
    void setAutoSystemThemeTracking(bool enable);

public slots:
    void onSystemColorSchemeChanged(Qt::ColorScheme scheme);

private:
    void setupDefaultPalettes();
    void parsePaletteFromJson(const QJsonObject &jsonObj, QPalette &palette);
    bool m_autoTracking;
    QPalette m_lightPalette;
    QPalette m_darkPalette;
};

#endif // ARRERA_THEME_CPP
