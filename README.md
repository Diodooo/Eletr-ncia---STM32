# Mini Display Automotivo

O arquivo main.c upado é apenas para visualização, ele já esta dentro da pasta zipada.

O projeto busca simular algumas funções de um carro e exibir em um display Nokia 5110 informações oriundas dessas funções.

- Centimetragem = Com o girar do Encoder e com sua circunferência é calculado um valor em centimetros de qual o deslocamento linear do mesmo. 
- Pisca= Ao girar o encoder para direita/esquerda pisca o LED correspondente ao lado do giro. Essa informação é exibida como Left/Right no display.
- Distância= Se refere ao sensor ultrassônico que atua como sensor de ré. Detecta a distâcnia de um obejto do sensor por meio de um sinal de controle PWM (duty-cycle = 10%(10us-90us)) e uma leitura com Input Capture + Cálculo na main.c. Ainda adicionei uma mensagem no display para informar de maneira geral o quão distante do sensor o objeto está, assim, despresando pequenas imprecisões do valor em centimetros.
