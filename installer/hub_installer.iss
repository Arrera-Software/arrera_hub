; ==========================================================
; SCRIPT INNO SETUP POUR ARRERA HUB
; ==========================================================

#define MyAppName "Arrera Hub"
#define MyAppVersion "I2026-0.00"
#define MyAppPublisher "Ton Nom ou Organisation"
#define MyAppExeName "Arrera_Hub.exe"
#define MyIconName "linux_win.ico"

; /!\ IMPORTANT : Remplace le chemin ci-dessous par le dossier de ton PC 
; où se trouvent tous les fichiers que tu m'as montrés (DLL, dossiers Qt, etc.)
; Garde bien le "\*" à la fin.
#define MyAppSourcePath "C:\Chemin\Vers\Ton\Dossier\De\Compilation\*"

[Setup]
; Identifiant unique de l'application (Généré par Inno Setup)
AppId={{A1B2C3D4-E5F6-G7H8-I9J0-K1L2M3N4O5P6}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}

; --- CONFIGURATION DU DOSSIER CIBLE ---
; Installe dans C:\Users\Nom\AppData\Local\Programs\Arrera Hub
DefaultDirName={localappdata}\Programs\{#MyAppName}

; FORCE le dossier : l'utilisateur ne verra pas la page pour changer le chemin
DisableDirPage=yes

; Pas besoin de droits administrateur (évite l'alerte de sécurité Windows)
PrivilegesRequired=lowest

; --- APPARENCE ET SORTIE ---
; Nom du fichier final généré
OutputBaseFilename=ArreraHub_Setup
; Dossier où sera créé l'installateur
OutputDir=.\Output
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

; L'icône affichée dans le panneau de configuration (Désinstallation)
UninstallDisplayIcon={app}\{#MyIconName}

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Files]
; On copie TOUT le contenu du dossier source vers le dossier d'installation {app}
; ignoreversion : écrase les anciennes versions
; recursesubdirs : inclut les dossiers 'platforms', 'styles', etc.
Source: "{#MyAppSourcePath}"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
; Création du raccourci dans le menu Démarrer
; On définit bien le WorkingDir ({app}) pour que l'app trouve ses DLL au lancement
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; IconFilename: "{app}\{#MyIconName}"

; (Optionnel) Raccourci sur le Bureau
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; IconFilename: "{app}\{#MyIconName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Run]
; Option pour lancer le logiciel immédiatement après l'installation
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent