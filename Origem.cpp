#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

using namespace cv;
using namespace std;

bool playButtonClicked = false;

//class Incrementacao
//{
//private:
//
//    int x, y, score, largura, altura, scoreIncremented;
//
//public:
//
//    Incrementacao(int _x, int _y, int _score, int _largura, int _altura, int _scoreIncremented) :
//        x(_x), y(_y), score(_score), largura(_largura), altura(_altura), scoreIncremented(_scoreIncremented) {};
//
//    void lopping(vector<Rect>faces, vector<Rect>rectangles)
//    {
//        score = 0;
//
//        for (const auto& faceRect : faces)
//        {
//            cv::Point tl(faceRect.x, faceRect.y);
//            cv::Point br(faceRect.x + faceRect.width, faceRect.y + faceRect.height);
//            bool notaCerta = false;
//
//            for (const auto& rect : rectangles)
//            {
//                cv::Rect intersection = rect & faceRect; // Calcula a interseção entre os retângulos
//
//                double intersectionArea = intersection.width * intersection.height; // Calcula a área da interseção
//                double faceRectArea = faceRect.width * faceRect.height; // Calcula a área do retângulo do rosto
//
//                double overlapPercentage = intersectionArea / faceRectArea; // Calcula a porcentagem de sobreposição
//
//                if (overlapPercentage >= 0.7) // Verifica se a sobreposição é maior ou igual a 70%
//                {
//                    if (x == 175 && rect.x == 0 && y == 625 && scoreIncremented != 0)
//                    {
//                        score = 1; // Incrementa a pontuação
//                        scoreIncremented = 0;
//                        notaCerta = true;
//                        cout << "0 incrementado" << endl;
//                        // Toca o som de acerto
//                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
//                    }
//
//                    else if (x == 500 && rect.x == largura && y == 625 && scoreIncremented != 1)
//                    {
//                        score = 1; // Incrementa a pontuação
//                        scoreIncremented = 1;
//                        notaCerta = true;
//                        cout << "1 incrementado" << endl;
//                        // Toca o som de acerto
//                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
//                        break;
//                    }
//
//                    else if (x == 850 && rect.x == largura * 2 && y == 625 && scoreIncremented != 2)
//                    {
//                        score = 1; // Incrementa a pontuação
//                        scoreIncremented = 2;
//                        notaCerta = true;
//                        cout << "2 incrementado" << endl;
//                        // Toca o som de acerto
//                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
//                    }
//
//                    else if (x == 1175 && rect.x == largura * 3 && y == 625 && scoreIncremented != 3)
//                    {
//                        score = 1; // Incrementa a pontuação
//                        scoreIncremented = 3;
//                        notaCerta = true;
//                        cout << "3 incrementado" << endl;
//                        // Toca o som de acerto
//                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
//
//                    }
//                    else if (!notaCerta && y >= 625)
//                    {
//                        //Toca o som de erro
//                        PlaySound((LPCWSTR)L"Saved Pictures/oboe-re.wav", NULL, SND_FILENAME | SND_ASYNC);
//                    }
//                }
//            }
//        }
//    }
//    
//    int get_score()
//    {
//        return score;
//    }
//};

void onMouseClick(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        // Verifica se o clique ocorreu dentro da área do botão "JOGAR"
        Rect* playButtonRect = (Rect*)userdata;
        if (playButtonRect->contains(Point(x, y)))
        {
            playButtonClicked = true;
        }
    }
}

int main() {

    // Carrega o classificador Haar Cascade para detecção de rosto
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("haarcascade_frontalface_default.xml")) {
        std::cout << "Erro ao carregar o arquivo XML do Haar Cascade." << std::endl;
        return -1;
    }

    VideoCapture cap(0);

    cap.set(CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CAP_PROP_FRAME_HEIGHT, 1080);


    // Variável para armazenar o valor lido do arquivo
    int recorde;


    // Cria um objeto ifstream e abre o arquivo no modo de leitura
    std::ifstream arquivo("Recorde.txt");

    // Verifica se o arquivo foi aberto corretamente
    if (arquivo.is_open()) {
        // Lê o valor do arquivo e armazena na variável
        arquivo >> recorde;

        // Fecha o arquivo
        arquivo.close();
    }

    string path = "Saved Pictures/h";
    Mat img = imread(path);
    Mat imgResize;

    resize(img, imgResize, Size(), 0.7, 0.7);

    int imageWidth = imgResize.cols;
    int imageHeight = imgResize.rows;

    int rectWidth = imageWidth / 4; // Largura de cada retângulo
    int rectHeight = imageHeight; // Altura dos retângulos será igual à altura da imagem

    int rectY = 0; // Posição vertical dos retângulos (no topo da imagem)

    // Criar as coordenadas dos quatro retângulos
    vector<Rect> rectangles;
    rectangles.push_back(Rect(0, rectY, rectWidth, rectHeight));
    rectangles.push_back(Rect(rectWidth, rectY, rectWidth, rectHeight));
    rectangles.push_back(Rect(rectWidth * 2, rectY, rectWidth, rectHeight));
    rectangles.push_back(Rect(rectWidth * 3, rectY, rectWidth, rectHeight));

    Scalar color(0, 0, 0); // Cor dos retângulos (verde)
    int thickness = 2; // Espessura da linha do retângulo


    float noteSpeed = 150; // Velocidade de deslocamento das notas (ajuste conforme necessário)

    double startTime = cv::getTickCount(); // Tempo inicial em ticks

    int restartPosition = -170; // Posição vertical de reinício da nota

    int x, y;

    srand(time(0)); // Inicializa a semente aleatória com base no tempo atual

    x = rand() % 4; // Gera um número aleatório entre 0 e 3 para selecionar o valor de x

    switch (x) {
    case 0:
        x = 175; // Primeira posição horizontal
        break;
    case 1:
        x = 500; // Segunda posição horizontal
        break;
    case 2:
        x = 850; // Terceira posição horizontal
        break;
    case 3:
        x = 1175; // Quarta posição horizontal
        break;
    }

    int score = 0;

    // Cria um retângulo para o botão "JOGAR"
    Rect playButtonRect(100, 150, 200, 100);
    Rect text(140, 350, 500, 150);

    // Adiciona o retângulo ao redor do botão "JOGAR"
    rectangle(imgResize, playButtonRect, Scalar(0, 0, 255), FILLED);
    rectangle(imgResize, text, Scalar(0, 0, 255), FILLED);

    // Escreve o texto "JOGAR" dentro do botão
    putText(imgResize, "JOGAR", Point(140, 205), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), thickness);
    putText(imgResize, "OBS: Para sair do jogo e ", Point(140, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), thickness);
    putText(imgResize, "salvar seu Recorde aperte 'q' ", Point(140, 450), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), thickness);

    imshow("Menu", imgResize);

    setMouseCallback("Menu", onMouseClick, &playButtonRect);

    while (!playButtonClicked)
    {
        char key = waitKey(1);
        if (key == 'q' || key == 27) // Sai do loop se a tecla 'q' ou 'Esc' for pressionada
            return 0;
    }

    int scoreIncremented = -1; // Variável para controlar se a pontuação já foi incrementada

    // ...

    std::vector<cv::Rect> faces;

    while (true) {

        Mat frame;
        cap >> frame;

        flip(frame, frame, 1);

        for (const auto& rect : rectangles)
        {
            cv::rectangle(frame, rect, color, thickness);
        }

        circle(frame, Point(175, 625), 40, Scalar(0, 255, 0), FILLED);
        circle(frame, Point(500, 625), 40, Scalar(0, 0, 255), FILLED);
        circle(frame, Point(850, 625), 40, Scalar(255, 0, 0), FILLED);
        circle(frame, Point(1175, 625), 40, Scalar(0, 255, 255), FILLED);


        faceCascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(30, 30));

        // Desenha um retângulo ao redor de cada rosto detectado
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }


        std::string scoreString = "Score: " + std::to_string(score);
        std::string recordeString = "Recorde: " + std::to_string(recorde);

        double currentTime = (cv::getTickCount() - startTime) / getTickFrequency(); // Tempo decorrido em segundos

        float verticalPosition = noteSpeed * currentTime; // Posição vertical da nota

        y = restartPosition + verticalPosition; // Posição vertical atualizada da nota

        if (y <= 650) {
            switch (x) {
            case 175:
                circle(frame, Point(x, y), 35, Scalar(0, 255, 0), FILLED); // Desenha a nota na posição atualizada
                break;
            case 500:
                circle(frame, Point(x, y), 35, Scalar(0, 0, 255), FILLED); // Desenha a nota na posição atualizada
                break;
            case 850:
                circle(frame, Point(x, y), 35, Scalar(255, 0, 0), FILLED); // Desenha a nota na posição atualizada
                break;
            case 1175:
                circle(frame, Point(x, y), 35, Scalar(0, 255, 255), FILLED); // Desenha a nota na posição atualizada
                break;
            }
        }
        else {
            startTime = cv::getTickCount(); // Reinicia o tempo
            restartPosition = 0; // Reinicia o y

            srand(time(0)); // Inicializa a semente aleatória com base no tempo atual

            x = rand() % 4; // Gera um número aleatório entre 0 e 3 para selecionar o valor de x

            switch (x) {
            case 0:
                x = 175; // Primeira posição horizontal
                break;
            case 1:
                x = 500; // Segunda posição horizontal
                break;
            case 2:
                x = 850; // Terceira posição horizontal
                break;
            case 3:
                x = 1175; // Quarta posição horizontal
                break;
            }
        }

        // Exibe a pontuação na imagem
        putText(frame, scoreString, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(frame, recordeString, Point(50, 80), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        imshow("Webcam", frame);

        // Verifica se a tecla 'q' foi pressionada para sair do programa
        if (waitKey(1) == 'q')
        {
            if (score > recorde)
            {
                std::ofstream arquivo("Recorde.txt");

                // Verifica se o arquivo foi aberto corretamente
                if (arquivo.is_open()) {
                    // Escreve o valor da variável no arquivo
                    arquivo << score;
                    // Fecha o arquivo
                    arquivo.close();
                }
            }
            break;
        }


       /* Incrementacao loop(x, y, score, rectWidth, rectHeight, scoreIncremented);

        loop.lopping(faces, rectangles);

        score += loop.get_score();*/


        for (const auto& faceRect : faces)
        {
            cv::Point tl(faceRect.x, faceRect.y);
            cv::Point br(faceRect.x + faceRect.width, faceRect.y + faceRect.height);
            bool notaCerta = false;

            for (const auto& rect : rectangles)
            {
                cv::Rect intersection = rect & faceRect; // Calcula a interseção entre os retângulos

                double intersectionArea = intersection.width * intersection.height; // Calcula a área da interseção
                double faceRectArea = faceRect.width * faceRect.height; // Calcula a área do retângulo do rosto

                double overlapPercentage = intersectionArea / faceRectArea; // Calcula a porcentagem de sobreposição

                if (overlapPercentage >= 0.7) // Verifica se a sobreposição é maior ou igual a 70%
                {
                    if (x == 175 && rect.x == 0 && y >= 625 && scoreIncremented != 0)
                    {
                        score++; // Incrementa a pontuação
                        scoreIncremented = 0;
                        notaCerta = true;
                        cout << "0 incrementado" << endl;
                        // Toca o som de acerto
                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
                    }

                    else if (x == 500 && rect.x == rectWidth && y >= 625 && scoreIncremented != 1)
                    {
                        score++; // Incrementa a pontuação
                        scoreIncremented = 1;
                        notaCerta = true;
                        cout << "1 incrementado" << endl;
                        // Toca o som de acerto
                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
                        break;
                    }

                    else if (x == 850 && rect.x == rectWidth * 2 && y >= 625 && scoreIncremented != 2)
                    {
                        score++; // Incrementa a pontuação
                        scoreIncremented = 2;
                        notaCerta = true;
                        cout << "2 incrementado" << endl;
                        // Toca o som de acerto
                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
                    }

                    else if (x == 1175 && rect.x == rectWidth * 3 && y >= 625 && scoreIncremented != 3 )
                    {
                        score++; // Incrementa a pontuação
                        scoreIncremented = 3;
                        notaCerta = true;
                        cout << "3 incrementado" << endl;
                        // Toca o som de acerto
                        PlaySound((LPCWSTR)L"Saved Pictures/trans-la.wav", NULL, SND_FILENAME | SND_ASYNC);
                    
                    }
                    else if (!notaCerta && y >= 625)
                    {
                        //Toca o som de erro
                        PlaySound((LPCWSTR)L"Saved Pictures/oboe-re.wav", NULL, SND_FILENAME | SND_ASYNC);
                    }
                }
            }
        }




        if (y <= 30) {
            // ...
            scoreIncremented = -1; // Define que a nota reiniciou, então a pontuação pode ser incrementada novamente
            cout << "looping" << endl;
        }
    }

    // ...


    destroyAllWindows();

    return 0;
}