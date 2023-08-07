// to compile the file please write this line: gcc ex3.c -pthread
// and then ./a.out conf"Name".txt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define ERROR -1
#define WORD_NOT_FOUND 4
#define SPORT 0
#define NEWS 1
#define WEATHER 2
#define NUM_OF_UNBOUNDED 3
#define NUM_OF_COEDITORS 3
#define MAX_SIZE 101
#define ID_SCREEN_QUEUE 4


typedef struct CoEditor
{
    char string[MAX_SIZE];
    int type;
    pthread_mutex_t m;
    pthread_mutex_t* superM;
    sem_t full;

} CoEditor;

typedef struct BoundedQueue
{
    int idProducer;
    char **queue;
    int size;
    // the limit of the number articles in the queue
    int maxSize;
    int bounder;
    int numSport;
    int numNews;
    int numWeather;
    pthread_mutex_t m;
    sem_t empty;
    sem_t full;

} BoundedQueue;

typedef struct UnBoundedQueue
{
    int type;
    char **queue;
    int size;
    int maxSize;

} UnBoundedQueue;

char **initQueue(int arrSize);
void BoundedInqueue(char **queue, int *size, char *article);
char *BoundedDequeue(char **queue, int *size);
BoundedQueue *createBoundedQueue(int bounder, int maxSize, int id);
void scanFile(char *file);
UnBoundedQueue *createUnBoundedQueue(int type);
void UnBoundedInqueue(char*** queue, int *size, char *article);
char *UnBoundedDequeue(char*** queue, int *size);

int BoundedArrSize = 0;
int amountOfArticles =0;
int endAll=0;
BoundedQueue **BoundedQueueArr = NULL;
BoundedQueue *ScreenManagerQueue = NULL;
UnBoundedQueue **UnBoundedQueueArr = NULL;
CoEditor** coEditorsArr = NULL;
int* count= NULL;
pthread_t* producers = NULL;
pthread_mutex_t superM;


void sleep_ms(int milliseconds) {
    unsigned int seconds = milliseconds / 1000;
    unsigned int microseconds = (milliseconds % 1000) * 1000;

    usleep(microseconds);
    sleep(seconds);
}

void deleteBoundedQueue (int x){
    if(BoundedQueueArr[x]->queue!=NULL){
    int length = BoundedQueueArr[x]->bounder;
    for (int i = 0; i < length; i++)
    {
        
        free(BoundedQueueArr[x]->queue[i]);
    }
    sem_destroy(&(BoundedQueueArr[x]->empty));
    sem_destroy(&(BoundedQueueArr[x]->full));
    pthread_mutex_destroy(&(BoundedQueueArr[x]->m));
    free(BoundedQueueArr[x]->queue);
    free(BoundedQueueArr[x]);
    } else{
    free(BoundedQueueArr[x]);
    }
}

void deleteBoundedQueueArray (){
    if(BoundedQueueArr!=NULL){
        for (int i = 0; i < BoundedArrSize; i++)
        {
            if(BoundedQueueArr[i]!=NULL)
            {
                deleteBoundedQueue(i);
            }
        }
        free(BoundedQueueArr);
    }
}


void deleteScreenManager (){
    if(ScreenManagerQueue!=NULL){
        if(ScreenManagerQueue->queue!=NULL)
        {
            int length = ScreenManagerQueue->bounder;
            for (int i = 0; i < length; i++)
            {
                free(ScreenManagerQueue->queue[i]);
            }
            sem_destroy(&(ScreenManagerQueue->empty));
            sem_destroy(&(ScreenManagerQueue->full));
            pthread_mutex_destroy(&(ScreenManagerQueue->m));
            free(ScreenManagerQueue->queue);
            free(ScreenManagerQueue);
        }
        else
        {
            free(ScreenManagerQueue);
        }
    }
}

void deleteUnBoundedQueueArr(){
    if (UnBoundedQueueArr!=NULL)
    {
        for (int i = 0; i < NUM_OF_UNBOUNDED; i++)
        {
            if(UnBoundedQueueArr[i]!=NULL)
            {
                free(UnBoundedQueueArr[i]);
            }
        }
        free(UnBoundedQueueArr);
    }
}

void deleteCoEditorsArr(){
    if (coEditorsArr!=NULL)
    {
        for (int i = 0; i < NUM_OF_COEDITORS; i++)
        {
            if (coEditorsArr[i]!=NULL)
            {
                sem_destroy(&(coEditorsArr[i]->full));
                pthread_mutex_destroy(&(coEditorsArr[i]->m));
                free(coEditorsArr[i]);
            }
        }

        free(coEditorsArr); 
    }
}

void freeAll(){

    deleteBoundedQueueArray();
    deleteScreenManager();
    deleteUnBoundedQueueArr();
    deleteCoEditorsArr();
    if(count!=NULL){
        free(count);
    }
    if(producers!=NULL){
        free(producers);
    }
    pthread_mutex_destroy(&(superM));
}

CoEditor* initCoEditor(int type){
    CoEditor* editor = (CoEditor*) malloc(sizeof(CoEditor));
    if(editor == NULL){
        freeAll();
        exit(ERROR);
    }
    editor->type = type;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        editor->string[i] = '\0';
    }
    pthread_mutex_init(&(editor->m), NULL);
    sem_init(&(editor->full), 0, 0);
    editor->superM = &superM;
    
}

void initCoEditorsArr()
{
    coEditorsArr = (CoEditor **)malloc(3 * sizeof(CoEditor *));
    if(coEditorsArr == NULL){
        freeAll();
        exit(ERROR);
    }
    for (int i = 0; i < NUM_OF_UNBOUNDED; i++)
    {
        coEditorsArr[i] = initCoEditor(i);
    }
}

char **initQueue(int arrSize)
{
    char **queue = (char **)malloc(arrSize * sizeof(char *)); // arr of articles
    if(queue == NULL){
        freeAll();
        exit(ERROR);
    }
    for (int i = 0; i < arrSize; i++)
    {
        queue[i] = (char *)malloc(MAX_SIZE); // one article
        if(queue[i] == NULL)
        {
        freeAll();
        exit(ERROR);
        }
    }
    return queue;
}

void BoundedInqueue(char **queue, int *size, char *article)
{
    (*size)++;
    strcpy(queue[(*size) - 1], article);

}

char *BoundedDequeue(char **queue, int *size)
{
    char *article = (char *)malloc(MAX_SIZE);
    if(article == NULL)
        {
        freeAll();
        exit(ERROR);
        }
    strcpy(article, queue[0]);
    for (int i = 0; i < (*size) - 1; i++)
    {
        strcpy(queue[i], queue[i + 1]);
    }
    strcpy(queue[(*size) - 1], "\0");
    (*size)--;
    return article;
}

BoundedQueue *createBoundedQueue(int bound, int maxSize, int id)
{
    BoundedQueue *boundedQueue = (BoundedQueue *)malloc(sizeof(BoundedQueue));
    if(boundedQueue == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    boundedQueue->size = 0;
    boundedQueue->maxSize = maxSize;
    boundedQueue->bounder = bound;
    boundedQueue->queue = initQueue(bound);
    boundedQueue->idProducer = id;
    boundedQueue->numNews=0;
    boundedQueue->numSport=0;
    boundedQueue->numWeather=0;
    pthread_mutex_init(&(boundedQueue->m), NULL);
    sem_init(&(boundedQueue->empty), 0, boundedQueue->bounder);
    sem_init(&(boundedQueue->full), 0, 0);
    return boundedQueue;
}


UnBoundedQueue *createUnBoundedQueue(int type)
{
    UnBoundedQueue *unBoundedQueue = (UnBoundedQueue *)malloc(sizeof(UnBoundedQueue));
    if(unBoundedQueue == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    unBoundedQueue->size = 0;
    unBoundedQueue->queue = NULL;
    unBoundedQueue->type = type;
    return unBoundedQueue;
}

void initUnBoundedQueue()
{
    UnBoundedQueueArr = (UnBoundedQueue **)malloc(NUM_OF_UNBOUNDED * sizeof(UnBoundedQueue *));
    if(UnBoundedQueueArr == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    for (int i = 0; i < NUM_OF_UNBOUNDED; i++)
    {
        UnBoundedQueueArr[i] = createUnBoundedQueue(i);
    }
}

void UnBoundedInqueue(char*** queue, int *size, char *article)
{
    if (!(*queue))
    {
        (*queue) = (char **)malloc(sizeof(char *));
        if((*queue) == NULL)
        {
            freeAll();
            exit(ERROR);
        }
    }
    else
    {
        (*queue) = (char **)realloc((*queue), ((*size) + 1) * sizeof(char *));
        if((*queue) == NULL)
        {
            freeAll();
            exit(ERROR);
        }
    }
    (*queue)[*size] = (char *)malloc(MAX_SIZE);
    if((*queue) == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    strcpy((*queue)[*size], article);
    (*size)++;
}

char *UnBoundedDequeue(char*** queue, int *size)

{
    //in order to return the article that i poped from the queue
    char* article = (char*) malloc (MAX_SIZE);
    if(article == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    strcpy(article, (*queue)[0]);

    //check if this is the last article in the queue so we need to remove it
    if((*size)==1){
        free ((*queue)[0]);
        free (*queue);
        (*queue) = NULL;
    }
    else{
        for (int i = 0; i < (*size) - 1; i++)
        {
            strcpy((*queue)[i], (*queue)[i + 1]);
        }
        free ((*queue)[(*size)-1]);
        (*queue) = (char**) realloc ((*queue), ((*size)-1)*sizeof(char*));
        if((*queue) == NULL)
        {
            freeAll();
            exit(ERROR);
        }
    }
    (*size)--;
    return article;
}

void scanFile(char *file)
{
    int args = 0, bounder = 0, idProducer = 0, total = 0, screenQueue = 0;
    FILE *input = fopen(file, "r");
    if (input == NULL){
        exit(ERROR);
    }
    while ((args = fscanf(input, "%d\n%d\n%d\n\n", &idProducer, &total, &bounder)) && (args == 3))
    {
        // if the arr is empty
        if (BoundedQueueArr == NULL)
        {
            BoundedQueueArr = (BoundedQueue **)malloc(sizeof(BoundedQueue *));
            if(BoundedQueueArr == NULL)
            {
                freeAll();
                exit(ERROR);
            }
        }
        else
        {
            BoundedQueueArr = (BoundedQueue **)realloc(BoundedQueueArr, sizeof(BoundedQueue *) * (BoundedArrSize + 1));
            if(BoundedQueueArr == NULL)
            {
                freeAll();
                exit(ERROR);
            }
        }
        BoundedQueueArr[BoundedArrSize] = createBoundedQueue(bounder, total, idProducer-1);
        BoundedArrSize++;
        amountOfArticles += total;
    }
    //create the screen manager queue
    ScreenManagerQueue = createBoundedQueue(idProducer, amountOfArticles, ID_SCREEN_QUEUE);
    fclose(input);
}

void createArticle (int* numSport, int* numNews, int* numWeather, int i, char arr[])
{
    int num=0;
    num = rand() % 3;
    strcpy(arr, "Producer ");
    char producerId[MAX_SIZE];
    char numOfType[MAX_SIZE];
    sprintf(producerId, "%d", i);
    strcat(arr, producerId);
    switch (num) {
        case 0:
            sprintf(numOfType,"%d", (*numSport));
            strcat(arr, " SPORTS ");
            strcat(arr, numOfType);
            strcat(arr, "\n");
            (*numSport)++;
            break;
        case 1:
            sprintf(numOfType,"%d", (*numNews));
            strcat(arr, " NEWS ");
            strcat(arr, numOfType);
            strcat(arr, "\n");
            (*numNews)++;
            break;
        case 2:
            sprintf(numOfType,"%d", (*numWeather));
            strcat(arr, " WEATHER ");
            strcat(arr, numOfType);
            strcat(arr, "\n");
            (*numWeather)++;
            break;
        default:
            break;
    }
}
int DefineType(char* article){
    char* sport = "SPORTS";
    char* news = "NEWS";
    char* wather = "WEATHER";
    if((strstr(article,sport))!=NULL){
        return SPORT;
    }
    if((strstr(article,news))!=NULL){
        return NEWS;
    }
    if((strstr(article,wather))!=NULL){
        return WEATHER;
    }
    else{
        return WORD_NOT_FOUND;
    }
}
void* Producer (void* i){
    int *x = (int*)i;
    int index = *x;
    int numOfIteration = BoundedQueueArr[index]->maxSize;
    while (numOfIteration)
    {
        char article [MAX_SIZE]={0};
        createArticle(&(BoundedQueueArr[index]->numSport),&(BoundedQueueArr[index]->numNews), &(BoundedQueueArr[index]->numWeather),BoundedQueueArr[index]->idProducer,article);
        sem_wait(&(BoundedQueueArr[index]->empty));
        pthread_mutex_lock(&(BoundedQueueArr[index]->m));
        BoundedInqueue(BoundedQueueArr[index]->queue,&(BoundedQueueArr[index]->size),article);
        pthread_mutex_unlock(&(BoundedQueueArr[index]->m));
        sem_post(&(BoundedQueueArr[index]->full));
        numOfIteration--;
    }
}
void* InsertArticle(void* i){
    int *x = (int*)i;
    int index = *x;
    char* article = BoundedDequeue(BoundedQueueArr[index]->queue, &(BoundedQueueArr[index]->size));
    int type = DefineType(article);

    switch (type)
    {
    case SPORT:

            pthread_mutex_lock(&(coEditorsArr[type]->m));
            UnBoundedInqueue(&(UnBoundedQueueArr[type]->queue), &(UnBoundedQueueArr[type]->size), article);
            pthread_mutex_unlock(&(coEditorsArr[type]->m));
            sem_post(&(coEditorsArr[type]->full));
        break;

    case NEWS:
            pthread_mutex_lock(&(coEditorsArr[type]->m));
            UnBoundedInqueue(&(UnBoundedQueueArr[type]->queue), &(UnBoundedQueueArr[type]->size), article);
            pthread_mutex_unlock(&(coEditorsArr[type]->m));
            sem_post(&(coEditorsArr[type]->full));
        break;
    case WEATHER:
            pthread_mutex_lock(&(coEditorsArr[type]->m));
            UnBoundedInqueue(&(UnBoundedQueueArr[type]->queue), &(UnBoundedQueueArr[type]->size), article);
            pthread_mutex_unlock(&(coEditorsArr[type]->m));
            sem_post(&(coEditorsArr[type]->full));
        break;
    default:
        break;
    }
    free(article);
}
void* DispatcherIteration (void* i){
    int *x = (int*)i;
    int index = *x;
    sem_wait(&(BoundedQueueArr[index]->full));
    pthread_mutex_lock(&(BoundedQueueArr[index]->m));
    InsertArticle(&(count[index]));
    pthread_mutex_unlock(&(BoundedQueueArr[index]->m));
    sem_post(&(BoundedQueueArr[index]->empty));
    (BoundedQueueArr[index]->maxSize)--;
}   

void* ScreenManager(void* i){
    int iter=ScreenManagerQueue->maxSize;
    while (iter)
    {
        sem_wait(&(ScreenManagerQueue->full));
        pthread_mutex_lock(&(ScreenManagerQueue->m));
        char* str =BoundedDequeue((ScreenManagerQueue->queue),&(ScreenManagerQueue->size));
        printf("%s", str);
        pthread_mutex_unlock(&(ScreenManagerQueue->m));
        sem_post(&(ScreenManagerQueue->empty));
        iter--;
        free(str);
    }
    printf("DONE\n");
}

void* CoEditorInsert(void* i){
    int *x = (int*)i;
    int index = *x;
    sem_wait(&(ScreenManagerQueue->empty));
    pthread_mutex_lock(&(ScreenManagerQueue->m));
    BoundedInqueue((ScreenManagerQueue->queue), &(ScreenManagerQueue->size), coEditorsArr[index]->string);
    pthread_mutex_unlock(&(ScreenManagerQueue->m));
    sem_post(&(ScreenManagerQueue->full));
}



void* CoEditorMove(void* i){
    int *x = (int*)i;
    int index = *x;
    while (1)
    {
        
        sem_wait(&(coEditorsArr[index]->full));
        pthread_mutex_lock(&(coEditorsArr[index]->m));
        sleep_ms(100);
        char* x = UnBoundedDequeue(&(UnBoundedQueueArr[index]->queue),&(UnBoundedQueueArr[index]->size));
        strcpy(coEditorsArr[index]->string,x);
        //free the x string
        free(x);
        pthread_mutex_unlock(&(coEditorsArr[index]->m));
        if (!strcmp(coEditorsArr[index]->string,"DONE\n")){
            break;
        }
        pthread_mutex_lock(&superM);
        CoEditorInsert((void*)&count[index]);
        pthread_mutex_unlock(&superM);
        
    }
    
}
void* Dispatcher (){
    int iter=1;
    while (iter)
    {
        iter=0;
        for (int i = 0; (i < BoundedArrSize); i++)
        {
            if((BoundedQueueArr[i]->maxSize)>0){
                iter=1;
            }
            if((BoundedQueueArr[i]->size)>0){

                DispatcherIteration((void*)&(count[i]));
            }
        }
        
    }
    for (int i = 0; i < NUM_OF_UNBOUNDED; i++)
    {
        UnBoundedInqueue(&(UnBoundedQueueArr[i]->queue), &(UnBoundedQueueArr[i]->size),"DONE\n");
        sem_post(&(coEditorsArr[i]->full));
    }
}

int main(int argc, char *argv[]){

    initUnBoundedQueue();
    pthread_mutex_init(&superM, NULL);
    initCoEditorsArr();
    int x=0;

    scanFile(argv[1]);
    pthread_t screen;
    producers = (pthread_t*) malloc (BoundedArrSize*sizeof(pthread_t));
    if(producers == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    pthread_t dispatchers;
    pthread_t coEditors [NUM_OF_COEDITORS];

    count = (int*) malloc (BoundedArrSize*sizeof(int));
    if(count == NULL)
    {
        freeAll();
        exit(ERROR);
    }
    for (int i = 0; i < BoundedArrSize; i++)
    {
        count[i]=i;
        pthread_create(&(producers[i]), NULL, Producer, (void*)&(count[i]));
    }

    pthread_create(&(dispatchers), NULL, Dispatcher, (void*)&(x));

    for (int i = 0; i < NUM_OF_COEDITORS; i++)
    {
        count[i]=i;
        pthread_create(&(coEditors[i]), NULL, CoEditorMove, (void*)&(count[i]));
    }

    pthread_create(&(screen), NULL, ScreenManager, (void*)&(x));

    for (int i = 0; i < BoundedArrSize; i++)
    {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < BoundedArrSize; i++)
    {
        pthread_join(coEditors[i], NULL);
    }

    pthread_join(dispatchers, NULL);
    pthread_join(screen, NULL);

    freeAll();


return 0;
}