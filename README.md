# Modelo-Epistemologico-Epidemias
MODELO EPISTEMOLÓGICO DE EPIDEMIAS EM REDES ALEATÓRIAS (SIR + INFORMAÇÃO)

1) IDEIA
- Rede social: Erdős–Rényi G(N,p), com p = kmed/N.
- Estados por indivíduo:
  Biológico: S=0 (saudável), I=1 (infectado), R=2 (removido).
  Epistemológico: know=0/1 (não sabe / sabe se proteger).
- Dinâmica por passo (ordem):
  1. Informação: quem sabe (know=1) ensina vizinhos com prob. Ptell.
  2. Contágio: infectados passam doença aos vizinhos com prob. Pinfect.
     Se o vizinho sabe (know=1), risco reduz: Prevention * Pinfect.
  3. Remoção: infectados viram removidos com prob. Pdie.
  4. Salto quântico: saudáveis podem se infectar sem contato com prob. Ptunelar.

2) PARÂMETROS (no topo do arquivo .c)
- N, T, kmed, Pinfect, Pdie, Ptell, Prevention, Ptunelar.
- Reprodutibilidade: descomente SEED para fixar a semente do gerador aleatório.
- Saída: CSV com colunas: t,S,I,R

3) COMO COMPILAR E RODAR
gcc -O3 -std=c11 Amanda2_final_corrigido.c -o sim
./sim
# Gera arquivo: Epidemia-N{N}-Pinfect{...}-Ptell{...}-Pdie{...}-Prevention{...}-Ptunelar{...}.csv

4) O QUE ESPERAR DOS GRÁFICOS
- S(t) tende a cair.
- I(t) cresce até um pico e depois cai.
- R(t) cresce ao longo do tempo.
- Aumentar Prevention/Ptell reduz e atrasa o pico de I(t).
- Aumentar Ptunelar aumenta casos por ruído externo.

5) NOTAS
- Estrutura de dados: lista de adjacência (rápida e econômica).
- Loop termina antes se I=0 (encurta a simulação automaticamente).
- Arquivo de saída tem cabeçalho: t,S,I,R
- Código e comentários em português do Brasil.

6) RESUMO
Modelo criado por mim para avaliar dinâmica da informação e dinâmica de doenças em uma rede complexa. 
Dessa maneira consegue-se observar,medir, quantificar a eficácia a informação na prevenção e 
disseminação de doenças e pode ser utilizado para gerar novas políticas públicas direcionadas e 
mais  efetiva e otimizada na area das políticas públicas de prevenção.
