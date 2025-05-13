## Twitch LED Bot - ESP8266

Criado por Eng. Renato Campoy.

Este projeto permite que você controle um strip de LED NeoPixel via comandos no chat da Twitch, utilizando a placa ESP8266. O código é escrito em C++ e utiliza a biblioteca WiFiManager para gerenciar a conexão Wi-Fi e a biblioteca Adafruit NeoPixel para controlar o strip de LED.

<p float="center">
 <img src="https://raw.githubusercontent.com/renatocampoy/Twitch-LED-Bot/refs/heads/main/tlb.png" />
</p>

## Token da Twitch

Para usar este código, você precisará de um token de OAuth da Twitch. Aqui estão as etapas para obtê-lo:

1.  Acesse [https://twitchapps.com/tmi/](https://twitchapps.com/tmi/)
2.  Clique em 'Connect' e autorize o acesso ao seu canal.
3.  Copie o token gerado. Este é o seu token OAuth, que permite que o bot envie mensagens ao seu canal.

Tenha em mente que este token é equivalente à senha da sua conta da Twitch. Não compartilhe com ninguém.

### Dependências

Você precisará das seguintes bibliotecas para usar este projeto:

-   ESP8266WiFi
-   DNSServer
-   ESP8266WebServer
-   WiFiManager
-   EEPROM
-   Adafruit_NeoPixel

Estas bibliotecas podem ser instaladas através do Gerenciador de Bibliotecas do Arduino.

## Instalação

1.  Clone este repositório.
2.  Abra o arquivo `.ino` no Arduino IDE.
3.  Carregue o código para o ESP8266.

## Configuração

1.  Ao ligar o ESP, uma rede wifi com o nome LEDBOT_AP aparecerá, conecte-se a ela.
2.  Ao conectar, aparecerá uma pagina onde você deve clicar em "Configure WIFI"
3.  Clique na sua REDE, entre com a senha.
4. Em "Nick" é o seu usuário da Twitch tudo em minúsculo 
5. Em Token você vai colocar o token gerado no tópico "Token da Twitch"

## Uso

Uma vez que o código esteja funcionando, você poderá enviar comandos para o chat da Twitch para controlar a iluminação do strip de LED. Aqui estão alguns exemplos de comandos que você pode usar:

-   `!led vermelho`: muda a cor do LED para vermelho.
-   `!led #ff00ff`: muda a cor do LED para o valor hexadecimal fornecido (neste caso, magenta).
-   `!led cores`: lista todas as cores disponíveis.

Além disso, o bot responde a comandos enviados via interface serial para reiniciar o ESP ou redefinir as configurações de WiFi ou Twitch. (vide código)

## Contribuição

Este é um projeto open-source. Se tiver alguma sugestão ou melhoria, por favor, faça um fork do repositório e envie uma pull request.

## Problemas Conhecidos

Atualmente, o bot não responde a erros do servidor da Twitch. Se você experimentar algum problema, tente reiniciar o ESP.

## Licença

Este projeto está licenciado sob a Licença GPLv3 - veja o arquivo [LICENSE](https://www.gnu.org/licenses/gpl-3.0.pt-br.html) para mais detalhes.
