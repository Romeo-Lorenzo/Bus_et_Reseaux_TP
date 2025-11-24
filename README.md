# Compte Rendu TP – Bus et Réseaux Embarqués

> **Module** : Bus et Réseaux  
> **Année** : 2025-2026  
> **Réalisé par** : ROMEO - POMMERY  

---

## 1. Présentation

Objectifs généraux du module :
- Maîtriser les principaux bus de communication embarqués (I²C, UART, CAN)
- Mettre en œuvre des échanges entre microcontrôleurs (STM32) et ordinateurs monocartes (Raspberry Pi)
- Développer une API REST pour piloter un système embarqué à distance
- Réaliser une intégration complète de l’ensemble des bus

Matériel utilisé :
- Nucleo STM32 (L432KC ou F401RE selon promo)
- Capteur BMP280 (pression/température)
- Accéléromètre (souvent LIS3DH ou MPU6050
- Raspberry Pi Zero W
- Module CAN (MCP2551 + SN65HVD230 ou équivalent)
- Moteur DC + driver

---

## 2. TP 1 – Bus I²C

### 2.1. Capteur BMP280
- Présentation du capteur (adresse I²C, registres utiles)
- Schéma de câblage
- Datasheet résumé (table des registres de calibration)

### 2.2. Setup du STM32
- Configuration CubeMX (I2C1 en mode Fast Mode, GPIO SCL/SDA avec pull-up)
- Génération du code avec STM32CubeIDE
- Vérification des broches (oscilloscope/logic analyzer)

### 2.3. Communication I²C
- Écriture/lecture simple d’un registre
- Implémentation des fonctions `BMP280 init(), readTemperature(), readPressure()
- Gestion des coefficients de calibration
- Affichage des résultats sur UART (bonus : moniteur série)

### 2.4. Interfaçage de l’accéléromètre
- Capteur utilisé (ex : LIS3DH)
- Détection de mouvement / double-tap
- Configuration des interruptions (optionnel)

> **Captures** : Oscilloscope I²C, sortie console

---

## 3. TP 2 – Interfaçage STM32 ↔ Raspberry Pi

### 3.1. Mise en route du Raspberry Pi Zero
- Installation de Raspberry Pi OS Lite (Headless)
- Activation UART (config.txt + cmdline.txt)
- Test avec `minicom` ou `screen`

### 3.2. Port Série
- Câblage STM32 (USART2) → Raspberry (GPIO14/15)
- Configuration USART STM32 (115200 bauds, 8N1)
- Envoi de trames texte ou binaires depuis le STM32

### 3.3. Commande depuis Python
- Script Python avec `pyserial`
- Protocole simple (ex : "TEMP?" → réponse "23.45\r\n")
- Parsing des données et affichage/graphique (matplotlib)

---

## 4. TP 3 – Interface REST

### 4.1. Installation du serveur Python
- Utilisation de `Flask` ou `FastAPI`
- Lancement sur le Raspberry Pi (`0.0.0.0:5000`)

### 4.2. Première page REST
- Route `/` → page HTML simple
- Route `/api/sensor` → retour JSON des données capteurs

### 4.3. Nouvelles méthodes HTTP
- GET  → lecture des données
- POST → commande moteur (vitesse, sens)
- PUT  → calibration ou réglages

### 4.4. Et encore plus fort...
- Authentification basique ou token
- Swagger / ReDoc (si FastAPI)
- Accès depuis smartphone ou autre PC du réseau

---

## 5. TP 4 – Bus CAN

### 5.1. Pilotage du moteur
- Module CAN STM32 (bxCAN)
- Configuration CubeMX (500 kbps ou 1 Mbps)
- Envoi de trames pour contrôler vitesse et sens du moteur

### 5.2. Interfaçage avec le capteur
- Deux cartes STM32 communicant en CAN
- Une carte lit le BMP280 et envoie les données toutes les 500 ms
- L’autre carte reçoit et affiche sur UART ou pilote le moteur en fonction de la pression

> **Capture** : Analyseur CAN (Peak-Can, Saleae, etc.)

---

## 6. TP 5 – Intégration I²C - Serial - REST - CAN

Architecture finale du projet complet :
