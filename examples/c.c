#include <stdio.h>


#define clr printf("\033[2J") //画面クリア
#define location(x,y) printf("\033[%d;%dH" ,x,y) //カーソル位置を移動


const int X_SIZE=50;
const int Y_SIZE=20;


main(){
	int x;
		int y;
			char data[51][21];
				char scr[51][21];
					char str[60]="";
						int Birth;
							int Dies;
								
									clr;

										//初期データ作成

											for(y=1;y<Y_SIZE;y++){
													for(x=1;x<X_SIZE;x++){

																scr[x][y]='_';

																		}
																			}

																				//glidergunsの初期設定
																					scr[25][ 1]='#';
																						scr[23][ 2]='#';
																							scr[25][ 2]='#';
																								scr[13][ 3]='#';
																									scr[14][ 3]='#';
																										scr[21][ 3]='#';
																											scr[22][ 3]='#';
																												scr[35][ 3]='#';
																													scr[36][ 3]='#';
																														scr[12][ 4]='#';
																															scr[16][ 4]='#';
																																scr[21][ 4]='#';
																																	scr[22][ 4]='#';
																																		scr[35][ 4]='#';
																																			scr[36][ 4]='#';
																																				scr[1 ][ 5]='#';
																																					scr[2 ][ 5]='#';
																																						scr[11][ 5]='#';
																																							scr[17][ 5]='#';
																																								scr[21][ 5]='#';
																																									scr[22][ 5]='#';
																																										scr[1 ][ 6]='#';
																																											scr[2 ][ 6]='#';
																																												scr[11][ 6]='#';
																																													scr[15][ 6]='#';
																																														scr[17][ 6]='#';
																																															scr[18][ 6]='#';
																																																scr[23][ 6]='#';
																																																	scr[25][ 6]='#';
																																																		scr[11][ 7]='#';
																																																			scr[17][ 7]='#';
																																																				scr[25][ 7]='#';
																																																					scr[12][ 8]='#';
																																																						scr[16][ 8]='#';
																																																							scr[13][ 9]='#';
																																																								scr[14][ 9]='#';


																																																									while(1){
																																																											
																																																													for(y=1;y<Y_SIZE;y++){
																																																																for(x=1;x<X_SIZE;x++){

																																																																				data[x][y]=scr[x][y];

																																																																							}
																																																																									}


																																																																											//goto scr_print:

																																																																													for(y=1;y<Y_SIZE;y++){
																																																																																for(x=1;x<X_SIZE;x++){

																																																																																				//誕生: 死んでいるセルの周囲に3つの生きているセルがあれば次の世代では生きる（誕生する）。 

																																																																																								Birth = 0;
																																																																																												if (data[x][y]=='_'){
																																																																																																	if (data[x-1][y]=='#' ) Birth = Birth + 1;
																																																																																																						if (data[x+1][y]=='#' ) Birth = Birth + 1;
																																																																																																											if (data[x][y-1]=='#' ) Birth = Birth + 1;
																																																																																																																if (data[x][y+1]=='#' ) Birth = Birth + 1;
																																																																																																																					if (data[x+1][y+1]=='#' ) Birth = Birth + 1;
																																																																																																																										if (data[x+1][y-1]=='#' ) Birth = Birth + 1;
																																																																																																																															if (data[x-1][y+1]=='#' ) Birth = Birth + 1;
																																																																																																																																				if (data[x-1][y-1]=='#' ) Birth = Birth + 1;
																																																																																																																																									if (Birth==3 ) scr[x][y]='#';
																																																																																																																																													}

																																																																																																																																																	//維持: 生きているセルの周囲に2つか3つの生きているセルがあれば次の世代でも生き残る。 
																																																																																																																																																					//死亡: 上以外の場合には次の世代では死ぬ。

																																																																																																																																																									Dies = 0;
																																																																																																																																																													if (data[x][y]=='#' ){
																																																																																																																																																																		if (data[x-1][y]=='#' ) Dies = Dies + 1;
																																																																																																																																																																							if (data[x+1][y]=='#' ) Dies = Dies + 1;
																																																																																																																																																																												if (data[x][y-1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																	if (data[x][y+1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																						if (data[x+1][y+1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																											if (data[x+1][y-1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																																if (data[x-1][y+1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																																					if (data[x-1][y-1]=='#' ) Dies = Dies + 1;
																																																																																																																																																																																																										if (Dies<2 ) scr[x][y]='_';
																																																																																																																																																																																																															if (Dies>3 ) scr[x][y]='_';
																																																																																																																																																																																																																			}
																																																																																																																																																																																																																						}
																																																																																																																																																																																																																								}

																																																																																																																																																																																																																										for(y=1;y<Y_SIZE;y++){
																																																																																																																																																																																																																													for(x=1;x<X_SIZE;x++){

																																																																																																																																																																																																																																	str[x-1]=scr[x][y];
																																																																																																																																																																																																																																				}
																																																																																																																																																																																																																																							location(y,1);
																																																																																																																																																																																																																																										printf("%s\n",str);
																																																																																																																																																																																																																																												}

																																																																																																																																																																																																																																														sleep(1);

																																																																																																																																																																																																																																															}

																																																																																																																																																																																																																																															}

