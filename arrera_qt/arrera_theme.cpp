#include "arrera_theme.h"

Arrera_Theme::Arrera_Theme(QObject *parent)
    : QObject(parent), m_autoTracking(true)
{
    QApplication::setStyle("fusion");

    setupDefaultPalettes();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
            this, &Arrera_Theme::onSystemColorSchemeChanged);

    applyTheme(qApp->styleHints()->colorScheme());
}

bool Arrera_Theme::loadThemeFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return false; // Fichier JSON invalide
    }

    QJsonObject rootObj = jsonDoc.object();

    // Parse la section "light"
    if (rootObj.contains("light") && rootObj["light"].isObject()) {
        parsePaletteFromJson(rootObj["light"].toObject(), m_lightPalette);
    }

    // Parse la section "dark"
    if (rootObj.contains("dark") && rootObj["dark"].isObject()) {
        parsePaletteFromJson(rootObj["dark"].toObject(), m_darkPalette);
    }

    // Réappliquer le thème actuel avec les nouvelles couleurs
    applyTheme(qApp->styleHints()->colorScheme());
    return true;
}

void Arrera_Theme::parsePaletteFromJson(const QJsonObject &jsonObj, QPalette &palette)
{
    // Dictionnaire pour lier les chaînes de caractères du JSON aux rôles Qt
    static const QMap<QString, QPalette::ColorRole> roleMap = {
        {"Window", QPalette::Window},
        {"WindowText", QPalette::WindowText},
        {"Base", QPalette::Base},
        {"AlternateBase", QPalette::AlternateBase},
        {"ToolTipBase", QPalette::ToolTipBase},
        {"ToolTipText", QPalette::ToolTipText},
        {"Text", QPalette::Text},
        {"Button", QPalette::Button},
        {"ButtonText", QPalette::ButtonText},
        {"Highlight", QPalette::Highlight},
        {"HighlightedText", QPalette::HighlightedText},
        {"PlaceholderText", QPalette::PlaceholderText},
        {"Midlight", QPalette::Midlight},
        {"Mid", QPalette::Mid}
    };

    // Parcourir toutes les clés de l'objet JSON (ex: "Window", "Text", etc.)
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString key = it.key();
        if (roleMap.contains(key)) {
            // Si la clé existe dans notre dictionnaire, on applique la couleur
            QColor color(it.value().toString());
            if (color.isValid()) {
                palette.setColor(roleMap[key], color);
            }
        }
    }
}

// --- RESTE DU CODE ADAPTÉ ---

void Arrera_Theme::applyTheme(Qt::ColorScheme scheme)
{
    // On utilise maintenant les palettes stockées en mémoire
    QPalette targetPalette = (scheme == Qt::ColorScheme::Dark) ? m_darkPalette : m_lightPalette;

    qApp->setPalette(targetPalette);

    for (QWidget *widget : QApplication::allWidgets()) {
        widget->setPalette(targetPalette);
        widget->update();
    }
}

void Arrera_Theme::setAutoSystemThemeTracking(bool enable)
{
    m_autoTracking = enable;
}

void Arrera_Theme::onSystemColorSchemeChanged(Qt::ColorScheme scheme)
{
    if (m_autoTracking) applyTheme(scheme);
}

void Arrera_Theme::setupDefaultPalettes()
{
    // === Palette Sombre (Fallback) ===
    m_darkPalette.setColor(QPalette::Window, QColor("#0E1116"));
    m_darkPalette.setColor(QPalette::WindowText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Base, QColor("#1E1E1E"));
    m_darkPalette.setColor(QPalette::AlternateBase, QColor("#12161F"));
    m_darkPalette.setColor(QPalette::ToolTipBase, QColor("#11151C"));
    m_darkPalette.setColor(QPalette::ToolTipText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Text, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Button, QColor("#11151C"));
    m_darkPalette.setColor(QPalette::ButtonText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Highlight, QColor("#6D5EFC"));
    m_darkPalette.setColor(QPalette::HighlightedText, QColor("#0B0F14"));

    // === Palette Claire (Fallback) ===
    m_lightPalette.setColor(QPalette::Window,         QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::WindowText,     QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Base,           QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::AlternateBase,  QColor("#F6F7FB"));
    m_lightPalette.setColor(QPalette::ToolTipBase,    QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ToolTipText,    QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Text,           QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Button,         QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ButtonText,     QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Highlight,      QColor("#4F46E5"));
    m_lightPalette.setColor(QPalette::HighlightedText,QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::PlaceholderText,QColor(14,23,38,140));
    m_lightPalette.setColor(QPalette::Midlight,       QColor("#EDF1F7"));
    m_lightPalette.setColor(QPalette::Mid,            QColor("#E3E8F0"));
}
