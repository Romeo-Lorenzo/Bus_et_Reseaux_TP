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
<img width="1092" height="431" alt="image" src="https://github.com/user-attachments/assets/71b65e79-b292-4c0e-81ea-42b388d55e30">

1. les adresses I²C possibles pour ce composant.
   
  0x76 (SDO -> GND) ou 0x77 (SDO -> VDDIO) (cf.p24)
   
2. le registre et la valeur permettant d'identifier ce composant
   
  le registre chip_id se trouve en 0xD0 et sa valeur de reset est 0x58 (cf. register map)
   
3. le registre et la valeur permettant de placer le composant en mode normal

  le registre 0xF4, il faut mettre le bit 0 et 1 à 1 (cf.p15)

4. les registres contenant l'étalonnage du composant
   
  registres 0x9E -> 0x88 (cf. p21)

5. les registres contenant la température (ainsi que le format)
   
  température sur 20bits: 8bits de 0xFA (MSB), 8bits de 0xFB (LSB), 7:4 du registre 0xFC (XLSB) (p.24)

6. les registres contenant la pression (ainsi que le format)
    
  pression sur 20bit: MSB sur 0xF7, LSB sur 0xF8, et XLSB sur 7:4 de 0xF9 (cf. p24)

7. les fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits.

<span style="color:#666; font-size:0.9em;">Extrait verbatim du datasheet Bosch (page 45-46) – version sans float, parfaite pour STM32</span>

<div style="background:#1e1e1e; padding:1px; border-radius:8px; border:1px solid #404040; margin:20px 0">

```c
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP2_S32_t t_fine;
BMP2_S32_t bmp280_compensate_T_int32(BMP2_S32_t adc_T)
{
    BMP2_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BMP2_S32_t)dig_T1<<1))) * ((BMP2_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BMP2_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP2_S32_t)dig_T1))) >> 12) *
           ((BMP2_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP2_U32_t bmp280_compensate_P_int64(BMP2_S32_t adc_P)
{
    BMP2_S64_t var1, var2, p;
    var1 = ((BMP2_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP2_S64_t)dig_P6;
    var2 = var2 + ((var1*(BMP2_S64_t)dig_P5)<<17);
    var2 = var2 + (((BMP2_S64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (BMP2_S64_t)dig_P3)>>8) + ((var1 * (BMP2_S64_t)dig_P2)<<12);
    var1 = (((((BMP2_S64_t)1)<<47)+var1))*((BMP2_S64_t)dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BMP2_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BMP2_S64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP2_S64_t)dig_P7)<<4);
    return (BMP2_U32_t)p;
}
```


### 2.2. Setup du STM32
- Configuration CubeMX (I2C1 sur broches PB8 et PB9)
- Ecriture d'un Driver I2C pour BMP280:

<span style="color:#666; font-size:0.9em;">Extrait verbatim du datasheet Bosch (page 45-46) – version sans float, parfaite pour STM32</span>

<div style="background:#1e1e1e; padding:1px; border-radius:8px; border:1px solid #404040; margin:20px 0">
'''c





------------------------------------ECRIS TON CODE ICI-----------------------------

```
- Vérification des broches (oscilloscope/logic analyzer)

### 2.3. Communication I²C
- Écriture/lecture simple d’un registre
Lecture du registre id: 0xD0

<span style="color:#666; font-size:0.9em;">Extrait verbatim du datasheet Bosch (page 45-46) – version sans float, parfaite pour STM32</span>

<div style="background:#1e1e1e; padding:1px; border-radius:8px; border:1px solid #404040; margin:20px 0">
'''c

  uint8_t result;

  bmp280_read_regs(&bmp280,BMP280_REG_CHIP_ID,&result,1);
  printf("id est %x",result);


HAL_StatusTypeDef bmp280_write_reg(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    return HAL_I2C_Master_Transmit(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),buf,2,HAL_MAX_DELAY);
}

HAL_StatusTypeDef bmp280_read_regs(BMP280_HandleTypeDef *dev,uint8_t reg,uint8_t *pData,uint16_t size)
{
    HAL_StatusTypeDef status;
    status = HAL_I2C_Master_Transmit(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),&reg,1,HAL_MAX_DELAY);
    if (status != HAL_OK){
        return status;
    }
    status = HAL_I2C_Master_Receive(&hi2c1,(BMP280_I2C_ADDR_SDO_HIGH << 1),pData,size,HAL_MAX_DELAY);
    return status;
}
```

On obtient alors le resultat suivant:

<img width="167" height="35" alt="image" src="https://github.com/user-attachments/assets/c229b956-eb97-43ef-992c-d83fff34ee63" />

- On envois maintenant la config au barometre donc oversamplig x2 de la temperature, oversampling x16 de la pression et mode normal.
  Pour cela nous allons lire puis ecrire une valeurs precise dans le registre 0xF4, on y applique les masque afin d'obtenir les bonnes valeurs oversampling et de mode de fonctionne en accord avec la datasheet. Puis pour lire les registre de calibration, on lit 24 octet, et on concatene certain de ces registres ensemble afin de former les valeurs de calibration finales en int16_t ou uint16_t.

<span style="color:#666; font-size:0.9em;">Extrait verbatim du datasheet Bosch (page 45-46) – version sans float, parfaite pour STM32</span>

<div style="background:#1e1e1e; padding:1px; border-radius:8px; border:1px solid #404040; margin:20px 0">
'''c
#define BMP280_MODE_NORMAL        0x03u
#define BMP280_OSRS_P_x16         (5u << 2)
#define BMP280_OSRS_T_x2          (2u << 5)
#define BMP280_REG_CALIB_START    0x88u
#define BMP280_CALIB_LENGTH       24u
                                         
uint8_t ctrl_meas = BMP280_OSRS_T_x2 | BMP280_OSRS_P_x16 | BMP280_MODE_NORMAL;
status = bmp280_write_reg(dev, BMP280_REG_CTRL_MEAS, ctrl_meas);

HAL_StatusTypeDef bmp280_read_calibration(BMP280_HandleTypeDef *dev)
{
    uint8_t calib[BMP280_CALIB_LENGTH];
    HAL_StatusTypeDef status;

    status = bmp280_read_regs(dev, BMP280_REG_CALIB_START, calib, BMP280_CALIB_LENGTH);
    if (status != HAL_OK) return status;

    dev->calib.dig_T1 = (uint16_t)((calib[1] << 8) | calib[0]);
    dev->calib.dig_T2 = (int16_t)((calib[3] << 8) | calib[2]);
    dev->calib.dig_T3 = (int16_t)((calib[5] << 8) | calib[4]);

    dev->calib.dig_P1 = (uint16_t)((calib[7] << 8) | calib[6]);
    dev->calib.dig_P2 = (int16_t)((calib[9] << 8) | calib[8]);
    dev->calib.dig_P3 = (int16_t)((calib[11] << 8) | calib[10]);
    dev->calib.dig_P4 = (int16_t)((calib[13] << 8) | calib[12]);
    dev->calib.dig_P5 = (int16_t)((calib[15] << 8) | calib[14]);
    dev->calib.dig_P6 = (int16_t)((calib[17] << 8) | calib[16]);
    dev->calib.dig_P7 = (int16_t)((calib[19] << 8) | calib[18]);
    dev->calib.dig_P8 = (int16_t)((calib[21] << 8) | calib[20]);
    dev->calib.dig_P9 = (int16_t)((calib[23] << 8) | calib[22]);

    return HAL_OK;
}
                                         
```
On obtient alors le resultat suivant pour ce qui est des registres de calibration:

- Maintenant nous allons faire la mesure de la temperature raw et de la pression raw et y appliquer es calibrations.


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
