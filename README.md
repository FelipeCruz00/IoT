# Irrigação Automática com ESP32
Este projeto foi criado para resolver o problema de irrigação de plantas de forma automática e barata. O uso do software é bem simples, basta compras as peças que iremos passar mais abaixo, entender o código antes de carregar ele na sua placa ESP32 e conectar os sensore de umidade de solo nas planntas desejadas.
Basicamente o que o software faz é: estabelecer bibliotecas e variáveis, estabelecer funções relacionado a conectar Wifi e MQTT, ler os valores atuais dos sensores, printar no monitor serial, enviar os dados coletados para o MQTT e repetir.
