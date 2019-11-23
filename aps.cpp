Autor: Pedro Henrique Guimarães Gomes - UTFPR 2019/2
RA: 2193000
Fisica Teórica 1 - S71 - Prof. Rafael C. Barreto


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define NAME "APS_SINUCA"
#define DIRETORIO_IMAGEM "bola.png" //caminho para o "sprite" da bola


typedef struct {

    double x;
    double y;

    double vx;
    double vy;

    int r;

}Bola;



int raio;

SDL_Window* window = NULL;

SDL_windower* render = NULL;

SDL_Surface* imagem_bola = NULL;

SDL_Texture* textura_bola = NULL;

SDL_Rect bola_atual;



//cria a janela e a superficie branca
void criaJanela(int WINDOW_WIDTH, int WINDOW_HEIGHT){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){

        printf("erro abrindo video\n");
    }
    else{

        window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if(window == NULL){

            printf("erro criando a window\n");
        }
        else{

            render = SDL_Createwindower(window, -1, SDL_windowER_ACCELERATED | SDL_windowER_PRESENTVSYNC);

            if(render == NULL){

                printf("erro criando render\n");
            }
            else{

                SDL_SetwindowDrawColor( render, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
}

//aplica a textura da bola
void aplicaTextura(){

    int imgFlags = IMG_INIT_PNG;

    if(!(IMG_Init(imgFlags) & imgFlags)){

        printf( "erro abrindo header imagem\n");
    }
    else{

        imagem_bola = IMG_Load(DIRETORIO_IMAGEM);

        if(imagem_bola == NULL){

            printf("erro abrindo imagem\n");
        }
        else{

            raio = imagem_bola->h /2;

            textura_bola = SDL_CreateTextureFromSurface(render, imagem_bola);

            if(textura_bola == NULL){

                printf("erro carregando textura\n");
            }
        }
    }

    SDL_FreeSurface(imagem_bola);
}

// renderiza as bolas em suas respectivas posições
void renderizaCena(Bola* bola, int n_bolas){

    for(int i = 0; i < n_bolas; i++){

        bola_atual.h = bola[i].r * 2;
        bola_atual.w = bola[i].r * 2;
        bola_atual.x = bola[i].x;
        bola_atual.y = bola[i].y;

        SDL_windowCopy(render, textura_bola, NULL, &bola_atual);
    }
}

// retorna o centro da bola
int achaCentro(int c){

    return (raio + c);
}

// checa se as bolas criadas estão sobrepostas
int sobreposicao(Bola* bola, int i, int j){

    double distancia_centros, x, y;

    x = achaCentro(bola[i].x) - achaCentro(bola[j].x);
    y = achaCentro(bola[i].y) - achaCentro(bola[j].y);
    distancia_centros = x*x + y*y;

    if(distancia_centros <= (pow(raio,2) * 4) + 5) 
        return 1;

    return 0;
}

//cria as bolas com parametros aleatórios
void criaBolas(Bola* bola, int n_bolas, int v_max, int v_min, int WINDOW_WIDTH, int WINDOW_HEIGHT){

    int i, j = 0;

    for(i = 0; i < n_bolas; i++){

        bola[i].r = raio;

        bola[i].vx = v_min + (rand() % (v_max - v_min));
        bola[i].vy = v_min + (rand() % (v_max - v_min));

        bola[i].x = (rand() % (WINDOW_WIDTH - (raio * 2))) + 1;
        bola[i].y = (rand() % (WINDOW_HEIGHT - (raio * 2))) + 1;
    }

    for(i = 0; i  < n_bolas; i++){

        for(j = i + 1; j < n_bolas; j++){

            if( sobreposicao(bola, i, j) == 1 ){

                bola[i].x = (rand() % (WINDOW_WIDTH - (raio * 2))) + 1;
                bola[i].y = (rand() % (WINDOW_HEIGHT - (raio * 2))) + 1;

                i = 0;
                j = 1;
            }
        }
    }
}


//atualiza velocidade apos colisao entre as bolas
void atualizaVelocidade (Bola* bola, int i, int j) {

    //eixo de colisao
    double colisao_x = achaCentro(bola[i].x) - achaCentro(bola[j].x);
    double colisao_y = achaCentro(bola[i].y) - achaCentro(bola[j].y);

    double colisao = colisao_x*colisao_x + colisao_y*colisao_y;

    //velocidades projetadas
    double pvx1 = ((bola[i].vx * colisao_x) + (bola[i].vy * colisao_y)) * colisao_x / colisao;
    double pvy1 = ((bola[i].vx * colisao_x) + (bola[i].vy * colisao_y)) * colisao_y / colisao;
    double pvx2 = ((bola[j].vx * colisao_x) + (bola[j].vy * colisao_y)) * colisao_x / colisao;
    double pvy2 = ((bola[j].vx * colisao_x) + (bola[j].vy * colisao_y)) * colisao_y / colisao;

    //inverte as velocidades que estao no EIXO de colisao
    bola[i].vx -= (pvx1 - pvx2);
    bola[i].vy -= (pvy1 - pvy2);
    bola[j].vx -= (pvx2 - pvx1);
    bola[j].vy -= (pvy2 - pvy1);

    //separa bolas grudadas
    if(colisao_x != 0 && colisao_y != 0){

        //soma ou diminui um pixel na posiçao das bolas
        bola[i].x += colisao_x / abs(colisao_x);
        bola[i].y += colisao_y / abs(colisao_y);

        bola[j].x -= colisao_x / abs(colisao_x);
        bola[j].y -= colisao_y / abs(colisao_y);
    }
}

// atualiza posição para cada iteração
void atualizaPosicao(Bola* bola, int n_bolas){

    for(int i = 0; i < n_bolas; i++){

        bola[i].x += bola[i].vx;
        bola[i].y += bola[i].vy;
    }
}

//confere se há colisões
void hitDetection(Bola* bola, int n_bolas, int WINDOW_WIDTH, int WINDOW_HEIGHT){
	
	    double distancia_centros;
    double x, y;

    for(int i = 0; i < n_bolas; i++){

        for(int j = i + 1; j < n_bolas; j++){

            x = achaCentro(bola[i].x) - achaCentro(bola[j].x);
            y = achaCentro(bola[i].y) - achaCentro(bola[j].y);
            distancia_centros = x*x + y*y;

            if(distancia_centros <= pow(raio,2) * 4)
                atualizaVelocidade(bola, i, j);

        }
    }

    for(int i = 0; i < n_bolas; i++){

        if((bola[i].x <= 0 && bola[i].vx <= 0 )|| (bola[i].x  + 2 * raio >= WINDOW_WIDTH && bola[i].vx >= 0))
            bola[i].vx *= -1;

        if((bola[i].y <= 0 && bola[i].vy <= 0 )||( bola[i].y + 2 * raio >= WINDOW_HEIGHT && bola[i].vy >= 0))
            bola[i].vy *= -1;
    }
}


int main(int argc, char** argv){

    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    int n_bolas;
    int v_max;
    int v_min;

    int executando = 1;

    SDL_Event fim;

    srand(time(NULL));

    switch(argc){

        case 1 :
            WINDOW_WIDTH = 800;
            WINDOW_HEIGHT = 600;
            n_bolas = 4;
            v_max = 5;
            printf("Primeiro argumento é a LARGURA. Segundo argumento é a ALTURA. Terceiro argumento é o NUMERO de BOLAS. Quarto argumento é a velocidade MAXIMA das BOLAS.\n");
            break;

        case 2 :
            WINDOW_WIDTH = atoi(argv[1]);
            WINDOW_HEIGHT = 600;
            n_bolas = 4;
            v_max = 5;
            printf("Primeiro argumento é a LARGURA. Segundo argumento é a ALTURA. Terceiro argumento é o NUMERO de BOLAS. Quarto argumento é a velocidade MAXIMA das BOLAS.\n");
            break;

        case 3 :
            WINDOW_WIDTH = atoi(argv[1]);
            WINDOW_HEIGHT = atoi(argv[2]);
            n_bolas = 4;
            v_max = 5;
            printf("Primeiro argumento é a LARGURA. Segundo argumento é a ALTURA. Terceiro argumento é o NUMERO de BOLAS. Quarto argumento é a velocidade MAXIMA das BOLAS.\n");
            break;

        case 4 :
            WINDOW_WIDTH = atoi(argv[1]);
            WINDOW_HEIGHT = atoi(argv[2]);
            n_bolas = atoi(argv[3]);
            v_max = 5;
            printf("Primeiro argumento é a LARGURA. Segundo argumento é a ALTURA. Terceiro argumento é o NUMERO de BOLAS. Quarto argumento é a velocidade MAXIMA das BOLAS.\n");
            break;

        default :
            WINDOW_WIDTH = atoi(argv[1]);
            WINDOW_HEIGHT = atoi(argv[2]);
            n_bolas = atoi(argv[3]);
            v_max = atoi(argv[4]);
            break;
    }

    if( WINDOW_HEIGHT < 100 || WINDOW_HEIGHT > 3000 || WINDOW_WIDTH < 100 || WINDOW_WIDTH > 3000 || n_bolas < 1 || n_bolas > 10 || v_max < 1 || v_max > 10 ){
            printf("\nDados Invalidos. Utilize valores de LARGURA e ALTURA entre 100 e 3000. Numero de bolas e velocidade maxima entre 1 e 10.\n");
            return 1;
    }

    v_min = v_max * (-1);

    Bola* bola = (Bola*) malloc (sizeof (Bola) * n_bolas);

    criaJanela(WINDOW_WIDTH, WINDOW_HEIGHT);

    aplicaTextura();

    criaBolas(bola, n_bolas, v_max, v_min, WINDOW_WIDTH, WINDOW_HEIGHT);

    printf("\nPressione a tecla \"x\" para fechar o programa.\n\n");


    while(executando == 1){

        while(SDL_PollEvent(&fim) != 0){
            // fecha quando pressiona tecla X
            if(fim.key.keysym.sym == SDLK_x)
                executando = 0;
        }


        // limpa a tela
        SDL_windowClear (render);

        // define a tela como branco
        SDL_SetwindowDrawColor ( render, 0xFF, 0xFF, 0xFF, 0xFF);

        // renderiza as bolas na tela
        renderizaCena(bola, n_bolas);

        // atualiza a posição das bolas somando a velocidade
        atualizaPosicao(bola, n_bolas);

        // checa se tem alguma colisão
        hitDetection(bola, n_bolas, WINDOW_WIDTH, WINDOW_HEIGHT);

        // mostra um novo window
        SDL_windowPresent (render);

        double energia_cinetica = 0;

        for(int i = 0; i < n_bolas; i++)
            energia_cinetica += (bola[i].vx*bola[i].vx) + (bola[i].vy*bola[i].vy);

        printf("Energia Cinética Total: %.2f\r", energia_cinetica/2);
    }

    //desaloca variaveis
    free(bola);
    SDL_DestroyTexture(textura_bola);
    SDL_Destroywindower(render);
    SDL_DestroyWindow(window);

    return (0);
}
