#!/bin/bash

# Script de sauvegarde pour Fork Knight
# Sauvegarde votre travail sur GitHub (branche main)
# Compatible travail en binôme

set +e  # Ne pas quitter en cas d'erreur (pour gérer les conflits)

echo "💾 Sauvegarde Pseudo-Kiosk"
echo "=========================================="
echo ""

# Vérifier que nous sommes dans un repo git
if [ ! -d .git ]; then
    echo "❌ Erreur : Ce n'est pas un dépôt Git"
    echo "   Exécutez ce script depuis la racine de votre projet"
    exit 1
fi

# Vérifier qu'on est sur main
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "main" ]; then
    echo "⚠️  Vous n'êtes pas sur la branche 'main' (actuellement sur '$CURRENT_BRANCH')"
    echo ""
    read -p "Voulez-vous retourner sur 'main' ? (o/N) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Oo]$ ]]; then
        git checkout main
    else
        echo "❌ Sauvegarde annulée"
        exit 1
    fi
fi

# 🆕 SYNCHRONISATION AVEC LE BINÔME
echo "🔄 Synchronisation"
echo ""

# Récupérer les informations du remote
git fetch origin main 2>/dev/null

# Vérifier s'il y a des modifications locales
HAS_LOCAL_CHANGES=false
if ! git diff-index --quiet HEAD -- 2>/dev/null; then
    HAS_LOCAL_CHANGES=true
fi

# Vérifier si le remote a des changements
LOCAL=$(git rev-parse main 2>/dev/null)
REMOTE=$(git rev-parse origin/main 2>/dev/null)

if [ "$LOCAL" != "$REMOTE" ]; then
    echo "📥 modifications. Récupération..."
    echo ""
    
    # Si on a des modifications locales, on les met de côté temporairement
    if [ "$HAS_LOCAL_CHANGES" = true ]; then
        echo "💾 Sauvegarde temporaire de vos modifications..."
        git stash push -m "Auto-stash before sync" >/dev/null 2>&1
        STASHED=true
    else
        STASHED=false
    fi
    
    # Tenter de récupérer les changements
    git pull origin main --no-edit 2>&1 | tee /tmp/git_pull_output.txt
    PULL_EXIT_CODE=${PIPESTATUS[0]}
    
    # Restaurer les modifications locales si elles ont été mises de côté
    if [ "$STASHED" = true ]; then
        echo ""
        echo "♻️  Restauration de vos modifications..."
        git stash pop >/dev/null 2>&1
        STASH_EXIT_CODE=$?
        
        if [ $STASH_EXIT_CODE -ne 0 ]; then
            echo ""
            echo "⚠️  ═══════════════════════════════════════════════"
            echo "⚠️  CONFLIT DÉTECTÉ !"
            echo "⚠️  ═══════════════════════════════════════════════"
            echo ""
            echo "modifié les mêmes fichiers."
            echo ""
            echo "📋 Fichiers en conflit :"
            git diff --name-only --diff-filter=U | sed 's/^/   • /'
            echo ""
            echo "🔧 Comment résoudre ?"
            echo ""
            echo "1. Ouvrez les fichiers listés ci-dessus"
            echo "2. Cherchez les marqueurs de conflit :"
            echo "   <<<<<<< HEAD"
            echo "   (new)"
            echo "   ======="
            echo "   (old)"
            echo "   >>>>>>> "
            echo ""
            echo "3. Choisissez quelle version garder (ou fusionnez)"
            echo "4. Supprimez les marqueurs <<<, ===, >>>"
            echo "5. Relancez : ./save.sh"
            echo ""
            echo "💡 Besoin d'aide ? skill issue lol"
            echo ""
            exit 1
        fi
    fi
    
    if [ $PULL_EXIT_CODE -eq 0 ]; then
        echo ""
        echo "✅ Synchronisation réussie"
        echo ""
    else
        # Vérifier s'il y a des conflits de merge
        if git diff --name-only --diff-filter=U | grep -q .; then
            echo ""
            echo "⚠️  ═══════════════════════════════════════════════"
            echo "⚠️  CONFLIT DÉTECTÉ !"
            echo "⚠️  ═══════════════════════════════════════════════"
            echo ""
            echo "file got conflict feeling about you and his old self"
            echo ""
            echo "📋 Fichiers en conflit :"
            git diff --name-only --diff-filter=U | sed 's/^/   • /'
            echo ""
            echo "🔧 Comment résoudre ?"
            echo ""
            echo "1. Ouvrez les fichiers listés ci-dessus"
            echo "2. Cherchez les marqueurs de conflit :"
            echo "   <<<<<<< HEAD"
            echo "   (your code)"
            echo "   ======="
            echo "   (other code)"
            echo "   >>>>>>> "
            echo ""
            echo "3. Choisissez quelle version garder (ou fusionnez)"
            echo "4. Supprimez les marqueurs <<<, ===, >>>"
            echo "5. Relancez : ./save.sh"
            echo ""
            echo "💡 Besoin d'aide ? get gud"
            echo ""
            exit 1
        fi
    fi
else
    echo "✅ Déjà synchronisé"
    echo ""
fi

# Vérifier s'il y a des modifications à sauvegarder
if git diff-index --quiet HEAD -- 2>/dev/null; then
    echo "✅ Aucune nouvelle modification à sauvegarder"
    echo ""
    echo "💡 Tout votre travail est déjà sauvegardé sur GitHub"
    exit 0
fi

# Afficher les fichiers modifiés
echo "📝 Fichiers modifiés :"
echo ""
git status --short
echo ""

# Demander un message de sauvegarde (optionnel)
if [ $# -eq 0 ]; then
    echo "💬 Message de sauvegarde (optionnel) :"
    echo "   Appuyez sur Entrée pour utiliser un message automatique"
    read -p "Message : " SAVE_MSG
    
    if [ -z "$SAVE_MSG" ]; then
        # Message automatique avec timestamp
        SAVE_MSG="save: $(date '+%d/%m/%Y %H:%M')"
    fi
else
    SAVE_MSG="$*"
fi

echo ""
echo "=========================================="
echo "📋 RÉCAPITULATIF"
echo "=========================================="
echo ""
echo "Branche          : main"
echo "Message          : $SAVE_MSG"
echo "Fichiers modifiés: $(git status --short | wc -l | tr -d ' ')"
echo ""
echo "=========================================="
echo ""

read -p "Confirmer la sauvegarde ? (o/N) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Oo]$ ]]; then
    echo "❌ Sauvegarde annulée"
    exit 1
fi

echo ""
echo "💾 Sauvegarde en cours..."
echo ""

# Ajouter tous les fichiers modifiés
git add .

# Commiter
git commit -m "$SAVE_MSG"

# Pousser vers GitHub
git push origin main

echo ""
echo "=========================================="
echo "✅ SAUVEGARDE RÉUSSIE !"
echo "=========================================="
echo ""
echo "Votre travail a été sauvegardé sur GitHub."
echo ""
echo "💡 Conseils :"
echo "   • Sauvegardez régulièrement (au moins 1x par session)"
echo "   • Utilisez ./save.sh après chaque fonction importante"
echo "   • Votre code est maintenant protégé contre les pertes"
echo ""
