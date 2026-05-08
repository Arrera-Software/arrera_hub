#!/bin/bash

# ==========================================
# VARIABLES À MODIFIER
# ==========================================
ZIP_URL="https://github.com/Arrera-Software/arrera_hub/releases/download/I2026-0.00/arrera-hub-linux-x86.zip" 
APP_NAME="arrera_hub"                              
EXECUTABLE_NAME="Arrera_Hub"                      
ICON_NAME="icon.png"                               
# ==========================================

DOWNLOADED_ZIP="/tmp/arrera-hub-linux-x86.zip"
FOLDER_NAME="arrera-hub-linux-x86"
TARGET_DIR="$HOME/Applications"
DESKTOP_DIR="$HOME/.local/share/applications"

echo "=== Installation d'Arrera Hub ==="

echo "Téléchargement de l'archive..."
curl -sS -L -o "$DOWNLOADED_ZIP" "$ZIP_URL"

if [ ! -f "$DOWNLOADED_ZIP" ]; then
    echo "Erreur : Le téléchargement a échoué."
    exit 1
fi

mkdir -p "$TARGET_DIR"

echo "Extraction des fichiers..."
unzip -q -o "$DOWNLOADED_ZIP" -d "$TARGET_DIR"

if [ $? -ne 0 ]; then
    echo "Erreur d'extraction."
    rm -f "$DOWNLOADED_ZIP"
    exit 1
fi

LAUNCH_SCRIPT="$TARGET_DIR/$FOLDER_NAME/launch.sh"
echo "Configuration du lancement..."
cat <<EOF > "$LAUNCH_SCRIPT"
#!/bin/bash
cd "\$(dirname "\$0")"
./$EXECUTABLE_NAME "\$@"
EOF

chmod +x "$LAUNCH_SCRIPT"
chmod +x "$TARGET_DIR/$FOLDER_NAME/$EXECUTABLE_NAME"

echo "Création du raccourci..."
mkdir -p "$DESKTOP_DIR"

cat <<EOF > "$DESKTOP_DIR/$APP_NAME.desktop"
[Desktop Entry]
Name=Arrera Hub
Comment=Gestionnaire d'installation
Exec="$LAUNCH_SCRIPT"
Icon=$TARGET_DIR/$FOLDER_NAME/$ICON_NAME
Terminal=false
Type=Application
Categories=Utility;
EOF

update-desktop-database "$DESKTOP_DIR" 2>/dev/null

rm -f "$DOWNLOADED_ZIP"

echo "=== Terminé ! Arrera Hub est installé. Et prêt à être utilisé. ==="