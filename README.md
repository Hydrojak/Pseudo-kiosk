# 🖥️ Pseudo-Kiosk Project (Windows)

**Pseudo-Kiosk** est un projet expérimental en C (WinAPI) visant à simuler un
*kiosk applicatif* sous Windows, sans modification du système.

Le principe est simple :

- l’utilisateur est bloqué dans une application plein écran
- il ne peut en sortir que lorsqu’une condition précise est remplie

---

## 🎯 Objectifs du projet

- Comprendre la WinAPI et la gestion des fenêtres Windows
- Simuler un mode kiosk en userland
- Bloquer les fermetures classiques (`Alt + F4`, croix, etc.)
- Forcer une interaction utilisateur avant sortie
- Explorer plusieurs mécanismes de libération

---

## 📦 Contenu du projet

Le dépôt contient **trois exécutables** :

- `launcher.exe`
- `kiosk.exe`
- `pong_kiosk.exe`

---

## 🚀 launcher.exe — Lanceur portable (USB)

### Fonctionnement

1. Copie l’exécutable cible (`kiosk.exe` ou `pong_kiosk.exe`) dans `%TEMP%`
2. Lance la copie locale
3. Se ferme immédiatement

➡️ La clé USB peut être retirée pendant l’exécution.

---

## 🖥️ kiosk.exe — Pseudo-kiosk par mot de validation

Application plein écran qui impose la saisie d’un mot correct pour quitter.

### Fonctionnalités

- Fenêtre plein écran, sans bordure
- Toujours au premier plan
- Fermeture classique bloquée
- Champ de saisie + bouton **Valider**
- Sortie uniquement si le mot est correct

### Mot secret

Le mot est défini directement dans le code :

```c

static const wchar_t *SECRET = L"secret";
```

### Touches spéciales

| Touche    | Action                    |
|----------|---------------------------|
| Valider  | Vérifie le mot            |
| Esc      | Quitter (panic / debug)   |
| Alt + F4 | ❌ Bloqué                 |

---

## 🎮 pong_kiosk.exe — Mini-jeu bloquant

Un **Pong minimaliste** en plein écran servant de condition de sortie.

### Règle principale

> Tant que tu n’as pas **5 points**, tu restes bloqué dans le jeu.

### Fonctionnalités

- Plein écran, topmost
- Contrôle de la raquette avec **↑ / ↓**
- Score affiché à l’écran
- Sortie automatique quand le score cible est atteint

### Sécurités

| Action              | Effet                              |
|--------------------|-----------------------------------|
| Atteindre 5 points | Sortie automatique                 |
| P                  | Quitter immédiatement (sécurité)  |
| Esc                | Quitter (panic / debug)           |
| Alt + F4           | ❌ Bloqué                           |

---
## 📁 Structure du dépôt
```
Pseudo-kiosk/
├─ src/
│ 
├─ launcher.c
│ ├─ kiosk.c
│ └─ pong_kiosk.c
├─ header/
│ ├─ launcher.h
│ ├─ kiosk.h
│ └─ pong_kiosk.h
├─ CMakeLists.txt
└─ README.md
```
---

## 🛠️ Compilation

### Prérequis

- Windows
- CLion + MinGW ou MSVC
- CMake ≥ 3.20

### Exemple `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)
project(pseudo_kiosk C)

set(CMAKE_C_STANDARD 11)

include_directories(header)

add_executable(launcher
        src/launcher.c
        header/launcher.h)

add_executable(kiosk
        src/kiosk.c
        header/kiosk.h)

add_executable(pong_kiosk
        src/pong_kiosk.c
        header/pong_kiosk.h)

target_link_libraries(kiosk user32 gdi32)
target_link_libraries(pong_kiosk user32 gdi32)
```
---
### 🔐 Notes de sécurité

-Aucune persistance

-Aucun hook clavier bas niveau

-Aucune modification système

-Tout fonctionne en userland

Ce projet ne remplace pas le vrai mode kiosk Windows (Assigned Access, GPO, etc.).