/*------------------------------------------------------*
|							|
|  Autor: Pedro Henrique Guimarães Gomes - UTFPR 2019/2	|
|  RA: 2193000						|
|  Fisica Teórica 1 - S71 - Prof. Rafael C. Barreto	|
|							|
*/------------------------------------------------------*

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <D:/Dev/SDL2/include/SDL.h>
#include <D:/Dev/SDL2_image/include/SDL_image.h>

#define NAME "APS Colisoes"
#define TEXTURE_DIR "D:/aps/APS BARRETO/APS BARRETO/bola.png"

typedef struct {

	double x;
	double y;
	double vx;
	double vy;

	int r;
}Bola;

int raio;

SDL_Window* window = NULL;

SDL_Renderer* render = NULL;

SDL_Surface* imagem_bola = NULL;

SDL_Texture* textura_bola = NULL;

SDL_Rect bola_atual;

void criaJanela(int WINDOW_WIDTH, int WINDOW_HEIGHT) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("Erro ao abrir o video\n");

	else {
		window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL)
			printf("Erro ao criar a janela\n");

		else {
			render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (render == NULL)
				printf("Erro ao criar o render\n");

			else
				SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		}
	}
}

void aplicaTextura(void) {

	int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags))
		printf("Erro ao abrir o header da imagem\n");

	else {
		imagem_bola = IMG_Load(TEXTURE_DIR);

		if (imagem_bola == NULL)
			printf("Erro ao abrir a imagem\n");

		else {

			raio = imagem_bola->h / 2;

			textura_bola = SDL_CreateTextureFromSurface(render, imagem_bola);

			if (textura_bola == NULL)
				printf("Erro ao carregar a textura");

		}
	}

	SDL_FreeSurface(imagem_bola);
}

void renderizaCena(Bola* bola, int n_bolas) {

	for (int i = 0; i < n_bolas; i++) {

		bola_atual.h = bola[i].r * 2;
		bola_atual.w = bola[i].r * 2;
		bola_atual.x = bola[i].x;
		bola_atual.y = bola[i].y;

		SDL_RenderCopy(render, textura_bola, NULL, &bola_atual);
	}
}

int sobreposicao(Bola* bola, int a, int b) {

	double dist_centros, x, y;

	x = (bola[a].x + raio) - (bola[b].x + raio);
	y = (bola[a].y + raio) - (bola[b].y + raio);

	dist_centros = x * x + y * y;

	if (dist_centros <= (pow(raio, 2.0) * 4) + 5)
		return 1;

	return 0;
}

void criaBolas(Bola* bola, int n_bolas, int v_max, int v_min, int WINDOW_WIDTH, int WINDOW_HEIGHT) {

	int i, j = 0;

	for (i = 0; i < n_bolas; i++) {

		bola[i].r = raio;

		bola[i].vx = v_min + (rand() % (v_max - v_min));
		bola[i].vy = v_min + (rand() % (v_max - v_min));

		bola[i].x = (rand() % (WINDOW_WIDTH - (raio * 2))) + 1;
		bola[i].y = (rand() % (WINDOW_HEIGHT - (raio * 2))) + 1;
	}

	for (i = 0; i < n_bolas; i++) {

		for (j = i + 1; j < n_bolas; j++) {

			if (sobreposicao(bola, i, j)) {

				bola[i].x = (rand() % (WINDOW_WIDTH - (raio * 2))) + 1;
				bola[i].y = (rand() % (WINDOW_HEIGHT - (raio * 2))) + 1;

				i = 0;
				j = 1;
			}
		}
	}
}

void atualizaPosicao(Bola* bola, int n_bolas) {

	for (int i = 0; i < n_bolas; i++) {

		bola[i].x += bola[i].vx;
		bola[i].y += bola[i].vy;

	}
}


void atualizaVelocidade(Bola* bola, int a, int b) {

	//define o eixo de colisão
	double colisao_x = (bola[a].x + raio) - (bola[b].x + raio);
	double colisao_y = (bola[a].y + raio) - (bola[b].y + raio);

	double colisao = colisao_x * colisao_x + colisao_y * colisao_y;

	//projeta as componentes das velocidades no eixo de colisão
	double vx1_proj = ((bola[a].vx * colisao_x) + (bola[a].vy * colisao_y)) * colisao_x / colisao;
	double vy1_proj = ((bola[a].vx * colisao_x) + (bola[a].vy * colisao_y)) * colisao_y / colisao;
	double vx2_proj = ((bola[b].vx * colisao_x) + (bola[b].vy * colisao_y)) * colisao_x / colisao;
	double vy2_proj = ((bola[b].vx * colisao_x) + (bola[b].vy * colisao_y)) * colisao_y / colisao;

	//define as novas velovidades
	bola[a].vx -= (vx1_proj - vx2_proj);
	bola[a].vy -= (vy1_proj - vy2_proj);
	bola[b].vx -= (vx2_proj - vx1_proj);
	bola[b].vy -= (vy2_proj - vy1_proj);

	//separa bolas sobrepostas
	if (colisao_x != 0 && colisao_y != 0) {

		bola[a].x += colisao_x / fabs(colisao_x);
		bola[a].y += colisao_y / fabs(colisao_y);

		bola[b].x -= colisao_x / fabs(colisao_x);
		bola[b].y -= colisao_y / fabs(colisao_y);
	}

}

void hitDetection(Bola* bola, int n_bolas, int WINDOW_WIDTH, int WINDOW_HEIGHT) {

	double dist_centros, x, y;

	for (int i = 0; i < n_bolas; i++) {

		if ((bola[i].x <= 0 && bola[i].vx <= 0) || (bola[i].x + 2 * raio >= WINDOW_WIDTH && bola[i].vx >= 0))
			bola[i].vx *= -1;

		if ((bola[i].y <= 0 && bola[i].vy <= 0) || (bola[i].y + 2 * raio >= WINDOW_HEIGHT && bola[i].vy >= 0))
			bola[i].vy *= -1;

	}


	for (int i = 0; i < n_bolas; i++) {

		for (int j = i + 1; j < n_bolas; j++) {

			x = (bola[i].x + raio) - (bola[j].x + raio);
			y = (bola[i].y + raio) - (bola[j].y + raio);
			dist_centros = pow(x, 2.0) + pow(y, 2.0);

			if (dist_centros <= pow(raio, 2.0) * 4)
				atualizaVelocidade(bola, i, j);

		}
	}

}



int main(int argc, char** argv) {

	int WINDOW_WIDTH, WINDOW_HEIGHT;
	int n_bolas, v_max, v_min;
	double energia_cinetica = 0;

	int executando = 1;

	SDL_Event fim;

	srand(time(NULL));

	switch (argc) {

	case 1:
		WINDOW_WIDTH = 800;
		WINDOW_HEIGHT = 600;
		n_bolas = 4;
		v_max = 10;
		printf("1o parametro e a LARGURA. 2o parametro e a ALTURA. 3o parametro e o NUMERO de BOLAS. 4o parametro e a velocidade MAXIMA das BOLAS.\n");
		break;

	case 2:
		WINDOW_WIDTH = atoi(argv[1]);
		WINDOW_HEIGHT = 600;
		n_bolas = 4;
		v_max = 10;
		printf("1o parametro e a LARGURA. 2o parametro e a ALTURA. 3o parametro e o NUMERO de BOLAS. 4o parametro e a velocidade MAXIMA das BOLAS.\n");
		break;

	case 3:
		WINDOW_WIDTH = atoi(argv[1]);
		WINDOW_HEIGHT = atoi(argv[2]);
		n_bolas = 4;
		v_max = 10;
		printf("1o parametro e a LARGURA. 2o parametro e a ALTURA. 3o parametro e o NUMERO de BOLAS. 4o parametro e a velocidade MAXIMA das BOLAS.\n");
		break;

	case 4:
		WINDOW_WIDTH = atoi(argv[1]);
		WINDOW_HEIGHT = atoi(argv[2]);
		n_bolas = atoi(argv[3]);
		v_max = 10;
		printf("1o parametro e a LARGURA. 2o parametro e a ALTURA. 3o parametro e o NUMERO de BOLAS. 4o parametro e a velocidade MAXIMA das BOLAS.\n");
		break;

	default:
		WINDOW_WIDTH = atoi(argv[1]);
		WINDOW_HEIGHT = atoi(argv[2]);
		n_bolas = atoi(argv[3]);
		v_max = atoi(argv[4]);
		break;
	}

	if (WINDOW_HEIGHT < 100 || WINDOW_HEIGHT > 3000 || WINDOW_WIDTH < 100 || WINDOW_WIDTH > 3000
		|| n_bolas < 1 || n_bolas > 10 || v_max < 1 || v_max > 10) {

		printf("\n Parametros Invalidos.\nUtilize valores entre 100 e 3000 para altura e largura da tela e entre 1 e 10 para n de bolas e vel. maxima");
		return 1;
	}

	v_min = v_max * (-1);

	Bola* bola = (Bola*)malloc(sizeof(Bola) * n_bolas);

	criaJanela(WINDOW_WIDTH, WINDOW_HEIGHT);

	aplicaTextura();

	criaBolas(bola, n_bolas, v_max, v_min, WINDOW_WIDTH, WINDOW_HEIGHT);

	printf("\n PRESSIONE |X| PARA ENCERRAR O PROGRAMA\n\n");

	while (executando == 1) {

		while (SDL_PollEvent(&fim) != 0) {
			if (fim.key.keysym.sym == SDLK_x)
				executando = 0;
		}

		SDL_RenderClear(render);

		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

		renderizaCena(bola, n_bolas);

		atualizaPosicao(bola, n_bolas);

		hitDetection(bola, n_bolas, WINDOW_WIDTH, WINDOW_HEIGHT);

		SDL_RenderPresent(render);

		energia_cinetica = 0;

		for (int i = 0; i < n_bolas; i++)
			energia_cinetica += pow(bola[i].vx, 2.0) + pow(bola[i].vy, 2.0);

		printf("Energia Cinetica Total = %.2f\r", energia_cinetica / 2);
	}

	free(bola);
	SDL_DestroyTexture(textura_bola);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	return 0;
}
