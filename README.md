# 🌌 OpenCL-CSFML-Mandelbrot  

> **Exploration de la fractale de Mandelbrot en C avec OpenCL et CSFML.**  

## 🚀 Présentation  

**OpenCL-CSFML-Mandelbrot** est un projet démontrant l'utilisation de **OpenCL** pour accélérer le rendu de la fractale de **Mandelbrot** en **C**. Il utilise **CSFML** pour l'affichage et permet de comparer l'exécution CPU et GPU en temps réel.  

## ✨ Fonctionnalités  

- ⚡ **Calcul rapide avec OpenCL**  
- 🎨 **Affichage interactif en CSFML**  
- 🔀 **Basculer entre CPU et GPU à la volée**  
- 🔍 **Zoom et exploration dynamique de la fractale**  
- 🛠 **Facilement extensible et configurable**  

## 🔧 Compilation & Exécution  

### 📦 Prérequis  

- **CMake** pour la configuration du projet  
- **CSFML** (doit être installé dans `/usr/local`, sinon modifier `CMakeLists.txt`)  
- **OpenCL** (GPU compatible requis pour l’accélération)  
- **Make** ou **Xcode** pour la compilation  

### 🛠️ Compilation sous Xcode  
```bash
    script/cmake-xcode  
```

### 🛠️ Compilation avec Make  
```bash
    script/cmake-makefile  
```

### 🚀 Exécution  

Lance l'exécutable généré après la compilation.  

## 🎮 Commandes Clavier  

- **Z** : Zoom avant  
- **X** : Zoom arrière  
- **A** : Augmenter la précision du calcul  
- **S** : Diminuer la précision du calcul  
- **D** : Afficher les informations de débogage  
- **P** : Afficher la position de la caméra  
- **G** : Basculer entre **CPU et GPU (OpenCL)**  

## 🏗️ Améliorations prévues  

- 🔜 Ajout d’une **colorisation avancée** des fractales  
- 🔜 Optimisation du code OpenCL pour **meilleure performance GPU**  
- 🔜 Ajout d’un mode **exploration automatique**  

## 🤝 Contribuer  

Les contributions sont les bienvenues ! Pour proposer des améliorations, ouvre une **issue** ou un **pull request** 🚀.  

## 🧑‍💻 Auteur  

Développé avec ❤️ par [**kdridi**](https://github.com/kdridi).  
⭐ N’hésite pas à **starrer** le projet si tu le trouves utile ! 🎨😊  
