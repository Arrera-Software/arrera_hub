# **🚀 Projet Arrera : Gestionnaire de Distribution d'Applications**

Ce document sert de guide et de TODO-list pour le développement de l'outil de distribution d'applications de l'organisation **Arrera** en Rust.

## **🛠️ Phase 1 : Récupération des Informations (GitHub)**

L'objectif est d'interroger les dépôts de l'organisation pour connaître les versions disponibles.

* \[ \] **Configuration du client API** : Utiliser octocrab ou reqwest.
* \[ \] **Authentification** : Gérer les Personal Access Tokens (PAT) si les dépôts sont privés.
* \[ \] **Parsing des Releases** : Créer des structures Rust avec serde pour mapper le JSON de GitHub.
* \[ \] **Filtrage des Assets** : Identifier dynamiquement le bon fichier selon l'OS (ex: .zip pour Windows, .tar.gz pour Linux).

## **📥 Phase 2 : Téléchargement des Paquets**

Gestion du transfert de fichiers de GitHub vers la machine locale.

* \[ \] **Requêtes Asynchrones** : Utiliser reqwest avec tokio.
* \[ \] **Streaming** : Implémenter le téléchargement par flux pour ne pas saturer la RAM.
* \[ \] **Gestion Temporaire** : Télécharger dans un dossier temp avant l'extraction (via la crate tempfile).
* \[ \] **Indicateur de progression** : Calculer le pourcentage de téléchargement pour l'envoyer à l'interface graphique.

## **📂 Phase 3 : Installation et Chemins (Multi-OS)**

Déploiement des fichiers dans les répertoires appropriés.

* \[ \] **Abstraction des chemins** : Utiliser la crate directories pour obtenir les dossiers "Home" proprement.
* \[ \] **Logique par Plateforme** :
    * **Linux** : Installer dans \~/.local/share/arrera/applications/ (standard recommandé au lieu de /home/User/Application).
    * **Windows** : Installer dans C:\\Users\\User\\AppData\\Local\\Arrera\\Apps\\.
    * **macOS** : Installer dans /Applications ou \~/Applications.
* \[ \] **Décompression** : Intégrer les crates zip ou tar / flate2.
* \[ \] **Permissions (Unix)** : Rendre le binaire exécutable via std::fs::set\_permissions.

## **🚀 Phase 4 : Intégration Système (Raccourcis)**

Rendre les applications lancables depuis les menus systèmes.

* \[ \] **Linux (.desktop)** :
    * Générer un fichier texte dans \~/.local/share/applications/arrera-\[nom\].desktop.
    * Gérer les icônes.
* \[ \] **Windows (Raccourcis)** :
    * Créer des fichiers .lnk dans le Menu Démarrer via la crate mslink.
* \[ \] **macOS** :
    * Vérifier l'enregistrement dans le Launchpad.

## **🎨 Phase 5 : Interface Graphique (ICED)**

Création de l'expérience utilisateur.

* \[ \] **Architecture TEA** : Mettre en place la boucle State, Message, Update, View.
* \[ \] **Composants UI** :
    * Liste des applications disponibles.
    * Bouton "Installer / Mettre à jour".
    * Barre de progression visuelle.
* \[ \] **Gestion Async** : Utiliser iced::Subscription pour écouter l'état des téléchargements sans bloquer l'UI.
* \[ \] **Thème Arrera** : Personnaliser les couleurs et les polices de l'interface.

## **📚 Stack Technique (Résumé)**

| Fonction | Crate (Bibliothèque) |
| :---- | :---- |
| **Runtime Async** | tokio |
| **Interface Graphique** | iced |
| **Requêtes HTTP** | reqwest / octocrab |
| **Chemins Système** | directories |
| **Sérialisation** | serde / serde\_json |
| **Compression** | zip / tar |
| **Gestion Erreurs** | anyhow |

## **📝 Notes pour plus tard**

* Ajouter une vérification de signature numérique pour la sécurité.
* Gérer la désinstallation propre des applications.
* Système de mise à jour automatique de l'installeur lui-même.