#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

void Baslangic();         //semaforların initialize edildiği fonksiyondur.
void Berberler(void * parametre);   //berberlerin görevlerini yerine getirdiği fonksiyondur.
void Musteriler(void * parametre);  //müşterilerin görevlerini yerine getirdiği fonksiyondur.
void Saci_Kes(int id);         //berberin yaptığı işi söyleyen fonksiyondur.
void Saci_Kestir(int id);      //müşterinin yaptığı işi söyleyen fonksiyondur.
void Bekleme();            //müşterilerin farklı zamanlarda gelmeleri kontrolünü sağlayan fonksiyondur.

sem_t berberler, musteriler, mutex;  //semafor tanımlamaları

int m, n, musteriSayisi, i, beklemeKoltugu, uyuyanBerber;

int main(int argc, const char* argv[])
{
      //kullanıcı girişleri veya kullanıcı parametre girişleri
      printf("Berber koltuğu sayısını giriniz: ");
      scanf("%d", &m);
      printf("Bekleme koltuğu sayısını giriniz: ");
      scanf("%d", &n);
      printf("Müşteri sayısını giriniz: ");
      scanf("%d", &musteriSayisi);

      //müşteri sayısının 0 olma kontrolü ve berberlerin uyuması
      while (musteriSayisi == 0)
      {
           for (i = 0; i < m; i++)
                 printf("%d. Berber salonda ve uyuyor..\n", i + 1);
           printf("Müşteri sayısını tekrar giriniz: ");
           scanf("%d", &musteriSayisi);
      }

      beklemeKoltugu = n;
      uyuyanBerber = m;
      pthread_t berberler[m], musteriler[n];
      Baslangic();

      //berberler ve müşteriler threadlerinin oluşturulması
      for (i = 0; i < m; i++)
      {
           pthread_create(&berberler[i], NULL, (void*)Berberler, (void*)&i);
           sleep(1);
      }
      for (i = 0; i < musteriSayisi; i++)
      {
           pthread_create(&musteriler[i], NULL, (void*)Musteriler, (void*)&i);
           Bekleme(); //müşteri threadleri yaratılırken, gerçeğe uygunluğu sağlanması amacıyla bu fonksiyon kullanılmıştır.
      }
                


      for (i = 0; i < musteriSayisi; i++)
      {
            pthread_join(musteriler[i], NULL);
      }
      exit(EXIT_SUCCESS); //programı başarıyla sonlandırır.
}

void Baslangic()
{
      sem_init(&musteriler, 0, 0);
      sem_init(&berberler, 0, 0);
      sem_init(&mutex, 0, 1);
}

void Berberler(void * parametre)
{
      int index = *(int*)(parametre);
      printf("%d. Berber salonda ve uyuyor..\n", index + 1);
      while (1)     //müşteri geldikçe berberi uyandırabilsin, aksi halde berber uyumaya gider.
      {
           sem_wait(&berberler);                        //berber müşteriyi bekler.
           sem_wait(&mutex);
           printf("%d. Berber uyandı...\n", index + 1);     //gelen müşteri berberi uyandırır.
           sem_post(&mutex);
           sem_post(&musteriler);                         //berber müşteriyle haberleşir.
           Saci_Kes(index + 1);
      }
}

void Musteriler(void * parametre)
{
      sem_wait(&mutex);
      int index = *(int*)(parametre);
      printf("%d. Müşteri salona giriş yaptı..(Boştaki Bekleme koltuğu =%d,Uyuyan berber=%d)\n", index + 1, beklemeKoltugu, uyuyanBerber);
      if (uyuyanBerber > 0)  //uyuyan berberi uyandır.
      {
           uyuyanBerber--;
           printf("%d. Müşteri uyuyan berberi uyandırıyor..\n", index + 1);
           sem_post(&mutex);
           sem_post(&berberler);  //müşteri berberle haberleşir.
           sem_wait(&musteriler); //müşteri hizmet alır.
           Saci_Kestir(index + 1);
           uyuyanBerber++;
           printf("%d. Müşteri salonu terk etti ve berberi uyuma gitti..\n", index + 1);
      }
      else if (beklemeKoltugu <= n && beklemeKoltugu > 0)  //berberler çalışıyorsa, bekleme koltuklarını kontrol et.
      {
           beklemeKoltugu--;
           printf("%d. Müşteri berberi bekliyor.. \n", index + 1);
           sem_post(&mutex);
           sem_post(&berberler);  //müşteri berberle haberleşir.
           sem_wait(&musteriler); //müşteri hizmet alır.
           Saci_Kestir(index + 1);
           beklemeKoltugu++;
           printf("%d. Müşteri salonu terk etti ve berberi uyuma gitti..\n", index + 1);
       }
       else //salon tamamen doluysa, müşteri gider.
       {
            sem_post(&mutex);
            printf("%d. Müşteri salondan yer olmadığı için ayrıldı..\n", index + 1);
       }
       pthread_exit(0);
}


void Saci_Kes(int id)
{
     printf("%d. Berber saçı kesiyor..\n", id);
     sleep(1);
}

void Saci_Kestir(int id)
{
     printf("%d. Müşteri saçını kestiriyor..\n", id);
}

void Bekleme()
{
     int x = rand() % (250000 - 50000 + 1) + 50000;
     srand(time(NULL));
     usleep(x);     
}
