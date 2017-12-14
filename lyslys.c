#include <stdio.h>
#define ANTAL_TILSTANDE 16
#define HORIZONT 5

typedef struct tilstand{
  int vLys;
  int hLys;
  int hBilister;
  int vNordBilister;
  int vSydBilister;
  int pr[ANTAL_TILSTANDE][ANTAL_TILSTANDE];
} tilstand;

void findTilstande(tilstand state[ANTAL_TILSTANDE]); 
void prob(tilstand s[], double prWait[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double prChange[ANTAL_TILSTANDE][ANTAL_TILSTANDE]);
void getReward(tilstand state[], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE]);
double value_update(tilstand state[], double prAction[ANTAL_TILSTANDE][ANTAL_TILSTANDE], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double V_forrige, int fstate);
double traffic_strategy(tilstand state[], double prAction[ANTAL_TILSTANDE][ANTAL_TILSTANDE], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double values[ANTAL_TILSTANDE], int nstate);

int main(){
  tilstand state[ANTAL_TILSTANDE];
  double V_forrige;
  double values[ANTAL_TILSTANDE];
  double prWait[ANTAL_TILSTANDE][ANTAL_TILSTANDE];
  double prChange[ANTAL_TILSTANDE][ANTAL_TILSTANDE];
  int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE]; 
  double largestValue = 0.0;
  int i, o, g, H, k = 0;


  findTilstande(state);
  prob(state, prWait, prChange);
  getReward(state, reward);
  
  for(i = 0; i < ANTAL_TILSTANDE; i++){
    values[i] = 0;
  }
  
  for(i = 0; i < ANTAL_TILSTANDE; i++){
    for(int o = 0; o < ANTAL_TILSTANDE; o++){
      printf("%.2lf ", prWait[o][i]);
    }
    printf("\n");
  }
  printf("-------------------------------------------------------------------------------\n");
  for(i = 0; i < ANTAL_TILSTANDE; i++){
    for(int o = 0; o < ANTAL_TILSTANDE; o++){
      printf("%.2lf ", prChange[o][i]);
    }
    printf("\n");
  } 
  printf("-------------------------------------------------------------------------------\n");
  for(i = 0; i < ANTAL_TILSTANDE; i++){
    for(int o = 0; o < ANTAL_TILSTANDE; o++){
      printf("%3d ", reward[o][i]);
    }
    printf("\n");
  }
  printf("-------------------------------------------------------------------------------\n"); 
  
  
  for(i = 0; i < ANTAL_TILSTANDE; i++) {
  V_forrige = 0; 
    for(H = 0; H < HORIZONT; H++){
      printf("                                         V_FORRIGE: %lf\n", V_forrige);
      
      if (value_update(state, prChange, reward, V_forrige, i) > value_update(state, prWait, reward, V_forrige, i)) {
      values[i] += value_update(state, prChange, reward, V_forrige, i); 
      }
  	  else {
  	  values[i] += value_update(state, prWait, reward, V_forrige, i);
      }
      
      V_forrige = values[i]; 
    }
  }
int aheirheaeirjaw;
  for (i = 0; i < ANTAL_TILSTANDE; i++) {
    printf("---------> %lf <----------\n", values[i]); 
  }

  for (o = 0; o < ANTAL_TILSTANDE; o++) {
    printf("\nState %d:\n", o+1);
    if (traffic_strategy(state, prWait, reward, values, o) < traffic_strategy(state, prChange, reward, values, o)) {
      printf("For state %d: Change signal\n", o+1);
    }
    else {
      printf("For state %d: Wait\n", o+1); 
    }
  }

  for (i = 0; i < ANTAL_TILSTANDE; i++) {
    printf("\nState %d: \nHorisontal signal: %d\nVertikal signal: %d\nHorisontal biler: %d\nVertikal-nord biler: %d\nVertikal-syd biler: %d\n", 
          i+1, state[i].hLys, state[i].vLys, state[i].hBilister, state[i].vNordBilister, state[i].vSydBilister);
  }

  return 0;
}

void findTilstande(tilstand state[ANTAL_TILSTANDE]){
  int index = 0, vLys, hLys, hBilister, vNordBilister, vSydBilister;
  for(vLys = 0, hLys = 1; vLys < 2; vLys++, hLys--){
    for(hBilister = 0; hBilister < 2; hBilister++){
      for(vNordBilister = 0; vNordBilister < 2; vNordBilister++){
        for(vSydBilister = 0; vSydBilister < 2; vSydBilister++){
          state[index].hLys = hLys;
          state[index].vLys = vLys;
          state[index].hBilister = hBilister;
          state[index].vNordBilister = vNordBilister;
          state[index].vSydBilister = vSydBilister;        
          index++;
        }
      }
    }
  }
}

void prob(tilstand s[], double prWait[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double prChange[ANTAL_TILSTANDE][ANTAL_TILSTANDE]){
  int a, b;
  double vProcent = 0.2, hProcent = 0.05, sandsynlighed;

  for(a = 0; a < ANTAL_TILSTANDE; a++){
    for(b = 0; b < ANTAL_TILSTANDE; b++){
      sandsynlighed = 1;                                   /* Handlingen Vent */
      /* Der undersøges om en bil der holdt for rødt, er forsvundet efter handlingen 'Vent' */
      if(s[a].vLys == 0 && s[a].vSydBilister == 1 && s[b].vSydBilister == 0 || 
         s[a].vLys == 0 && s[a].vNordBilister == 1 && s[b].vNordBilister == 0 || 
         s[a].hLys == 0 && s[a].hBilister == 1 && s[b].hBilister == 0){ 
        sandsynlighed = 0; /* Dette kan ikke lade sig gøre, sandsynligheden er 0% */
      }else{ /* Hvis der ikke er forsvundet en bil for rødt lys */
        if(s[a].vLys == 0 && s[b].hBilister == 1){
          sandsynlighed *= hProcent;
        }else if(s[a].hBilister == 1 && s[b].hBilister == 1){
          sandsynlighed *= 1;
        }else if(s[b].hBilister > s[a].hBilister){
          sandsynlighed *= hProcent;
        }else{
          sandsynlighed *= 1 - hProcent;
        }

        if(s[a].vLys == 1 && s[b].vNordBilister == 1){
          sandsynlighed *= vProcent;
        }else if(s[a].vNordBilister == 1 && s[b].vNordBilister == 1){
          sandsynlighed *= 1;
        }else if(s[b].vNordBilister > s[a].vNordBilister){
          sandsynlighed *= vProcent;
        }else{
          sandsynlighed *= 1 - vProcent;
        }

        if(s[a].vLys == 1 && s[b].vSydBilister == 1){
          sandsynlighed *= vProcent;
        }else if(s[a].vSydBilister == 1 && s[b].vSydBilister == 1){
          sandsynlighed *= 1;
        }else if(s[b].vSydBilister > s[a].vSydBilister){
          sandsynlighed *= vProcent;
        }else{
          sandsynlighed *= 1 - vProcent;
        }
      }
    if(s[a].vLys == s[b].vLys){
      prWait[a][b] = sandsynlighed;
    }else{
      prChange[a][b] = sandsynlighed;
    }
    }
  } 
}

void getReward(tilstand state[], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE]) {
  int i, k; 
  for (i = 0; i < ANTAL_TILSTANDE; i++) {
    for (k = 0; k < ANTAL_TILSTANDE; k++) {
      if(state[k].hLys == 1){
        reward[i][k] = (state[i].hBilister + state[i].vNordBilister + state[i].vSydBilister) - (state[k].hBilister + state[k].vNordBilister + state[k].vSydBilister); 
        if ((state[i].vSydBilister == 0 && state[i].vNordBilister == 0 && state[i].hBilister == 0) && ((state[k].vSydBilister == 0 && state[k].vNordBilister == 0 && state[k].hBilister == 0))) {
          reward[i][k] = 1; 
        }
      }else{
        if ((state[i].vSydBilister == 0 && state[i].vNordBilister == 0 && state[i].hBilister == 0) && ((state[k].vSydBilister == 0 && state[k].vNordBilister == 0 && state[k].hBilister == 0))) {
          reward[i][k] = 2 * 1;  
        }
        else {
        reward[i][k] = 2 * ((state[i].hBilister + state[i].vNordBilister + state[i].vSydBilister) - (state[k].hBilister + state[k].vNordBilister + state[k].vSydBilister));
        }       
      }
    }
  }
}

double value_update(tilstand state[], double prAction[ANTAL_TILSTANDE][ANTAL_TILSTANDE], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double V_forrige, int fstate) {
  int nstate = 0;
  double max = -30, v = 0;

  for (nstate = 0; nstate < ANTAL_TILSTANDE; nstate++) {
    v = prAction[fstate][nstate] * (reward[fstate][nstate] + V_forrige); 
    
    if (v > max) {
      max = v; 
    } 
  } 
  return max;
}

double traffic_strategy(tilstand state[], double prAction[ANTAL_TILSTANDE][ANTAL_TILSTANDE], int reward[ANTAL_TILSTANDE][ANTAL_TILSTANDE], double values[ANTAL_TILSTANDE], int nstate) {
  double v = 0, max = -30; 
  int i;
  for (i = 0; i < ANTAL_TILSTANDE; i++) {
    v = prAction[nstate][i] * (reward[nstate][i] + values[i]); 
    /* printf("v = %lf * (%d + %lf)\n", prAction[nstate][i], reward[nstate][i], values[i]); */
    
    if (v > max) {
      max = v; 
    } 
  } 
  printf("%lf\n", max);
  return max;
}