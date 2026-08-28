# 🤖 Robô Sumô Controlado

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-blue)
![Arduino](https://img.shields.io/badge/IDE-Arduino-00979D)
![Robótica](https://img.shields.io/badge/área-robótica-red)
![License](https://img.shields.io/badge/license-MIT-green)

---

## 📌 Sobre o projeto

Este projeto consiste no desenvolvimento de um **robô sumô controlado remotamente**, voltado para competições de robótica.

O robô utiliza uma arquitetura baseada em **controle diferencial (tank drive)**, permitindo movimentos ágeis, precisos e com alto torque, fundamentais para estratégias de combate em arena.

---

## 🖼️ Imagens do projeto


### 🔧 Estrutura do robô
![Chassi]()

### ⚙️ Arquitetura do sistema
![Sistema]()

### 🔌 Circuito eletrônico
![Circuito]()

---

## 🧠 Tecnologias utilizadas

- ESP32 WROOM-32  
- Driver de ponte H (H-Bridge)  
- Motores DC  
- Estrutura mecânica personalizada (chassi)  
- Protoboard e jumpers  
- Arduino IDE  
- Comunicação sem fio via ESP32  

---

## 🎮 Controle do robô

O robô é controlado remotamente, permitindo:

- Controle independente dos dois motores
- Movimentos:
  - Frente / trás
  - Giro no próprio eixo
- Alta responsividade para combate

---

## 🎯 Objetivos

- Desenvolver um robô competitivo para batalhas de sumô  
- Aplicar conceitos de eletrônica e sistemas embarcados  
- Trabalhar com programação em microcontroladores (ESP32)  
- Integrar hardware + software em um sistema funcional  
- Melhorar coordenação de equipe em projetos de engenharia  

---

## ⚙️ Status do projeto

🚧 Em desenvolvimento  
- Integração do controle remoto  
- Testes de motores  
- Ajustes mecânicos do chassi  
- Estudo do controle diferencial  

---

## 🧩 Arquitetura do sistema

```mermaid
flowchart TD
A[Controle Remoto] --> B[ESP32]
B --> C[Driver Ponte H]
C --> D[Motor Esquerdo]
C --> E[Motor Direito]
