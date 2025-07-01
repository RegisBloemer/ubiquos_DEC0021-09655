# Artigos Relacionados ao tema do projeto.

## 1. Anomaly Detection Method for Rotating Machinery Monitoring Based on the Most Representative Data

**Resumo**  
Propõe um algoritmo de detecção de anomalias baseado em dados representativos para monitoramento contínuo de máquinas rotativas (ex.: ventiladores centrífugos). Utiliza acelerômetros piezoelétricos, extração de características de vibração e um método k-NN para identificar falhas reais em ambiente industrial. Compara resultados com técnicas tradicionais e apresenta casos de falhas e impacto da manutenção real.

**Relevância**  
Foco em aplicação prática, automação e análise de grandes volumes de dados típicos da Indústria 4.0.

---

## 2. Vibration-Based Anomaly Detection Using LSTM/SVM Approaches

**Resumo**  
Desenvolve um método semi-supervisionado para detecção de anomalias que combina LSTM com one-class SVM. Treinado apenas com dados de operação normal, detecta falhas em engrenagens e mancais, validado em um gearbox e dados de teste em voo de helicóptero.

**Relevância**  
Aplica Deep Learning avançado (LSTM) e análise de vibração, ideal para sistemas com poucos dados de falhas — algo comum na indústria.

---

## 3. Vibration-Based Anomaly Detection in Industrial Machines: A Comparison of Autoencoders and Latent Spaces

**Resumo**  
Estuda aprendizado profundo (autoencoders) para detecção de anomalias em dados de vibração de transportadores. Converte sinais em espectrogramas, realiza pré-processamento baseado em velocidade e cargas, e compara técnicas de detecção automática em diferentes condições.

**Relevância**  
Demonstra a transição dos sinais de vibração para imagens para análise com redes convolucionais, um approach moderno em Indústria 4.0.

---

## Comparação entre os Artigos

| Aspecto                     | Artigo 1 (k-NN)                | Artigo 2 (LSTM + SVM)           | Artigo 3 (Autoencoder)              |
|----------------------------|--------------------------------|----------------------------------|-------------------------------------|
| Técnica de DL/ML           | k-Nearest Neighbors            | LSTM + one-class SVM            | Autoencoder com espectrogramas      |
| Dados de falha             | Sim (industriais reais)        | Não (apenas dados normais)      | Simulados e reais                   |
| Aplicação industrial       | Ventiladores industriais       | Gearbox e helicóptero           | Transportadores industriais         |
| Pré-processamento          | Features manuais               | Séries temporais + estatísticas | FSST + conversão em imagem          |

---

## Referências

1. LIS, A.; DWORAKOWSKI, Z.; CZUBAK, P. An anomaly detection method for rotating machinery monitoring based on the most representative data. Journal of Vibroengineering, v. 23, n. 4, p. 861–876, 11 fev. 2021.

2. VOS, K. et al. Vibration-based anomaly detection using LSTM/SVM approaches. Mechanical Systems and Signal Processing, v. 169, p. 108752, abr. 2022.

3. RADICIONI, L.; BONO, F. M.; CINQUEMANI, S. Vibration-Based Anomaly Detection in Industrial Machines: A Comparison of Autoencoders and Latent Spaces. Machines, v. 13, n. 2, p. 139–139, 12 fev. 2025.
