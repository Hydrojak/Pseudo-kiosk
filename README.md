# 🚀 Pseudo-Kiosk (Windows)

**Pseudo-Kiosk** est une petite application Windows en C (WinAPI) qui simule un mode “kiosk” simple et contrôlé :

✔️ Fenêtre plein écran  
✔️ Toujours au premier plan  
✔️ `Alt + F4` et fermeture classique bloqués  
✔️ Sortie **uniquement** avec le bon mot + bouton **Valider**  
✔️ Possibilité de lancer depuis une **clé USB** via un launcher

> 🛡️ Projet créé pour l’apprentissage et les environnements contrôlés.

---

## 🎯 Fonctionnement

### 🖥️ `kiosk.exe`

Au démarrage :

1️⃣ crée une fenêtre plein écran  
2️⃣ affiche au centre :
- un **champ de saisie**
- un bouton **Valider**

👉 Le programme se ferme **uniquement** si le mot entré correspond au mot secret.

🔑 Mot secret par défaut (modifiable dans `kiosk.c`) :

```c
static const wchar_t *SECRET = L"secret";
