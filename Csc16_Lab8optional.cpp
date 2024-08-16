//STARRT OFF DRAAAGOOOON ROAAAAR
// Dragon and Slayer skeleton and base classes.
//requires std=c++17

#include<iostream>
#include<SFML/Graphics.hpp>
#include<vector>
#include<tuple>     
#include<ctime>
#include<cmath>
#include<set>
#include "animatedgif.cpp"
#include "Csc16:Lab8part2.cpp" 
#include<deque>   /* if you can't get your own circular queue to work */
using namespace std;

const int wwidth = 1024; // window dimensions in pixels
const int wheight = 680;

const int vgap = 24; // space between lines (squares)
const int hgap = 24; 

const int mwidth = wwidth/hgap;  // matrix dimensions in squares
const int mheight = wheight/vgap;

const char DRAGON = 8;
const char EMPTY = 0;
const char FOOD = 16;
const char SLAYER = -128;
const int WITHINSIGHT = 12;



typedef pair<int,int> coord;  // a coordinate is just a pair of ints

typedef circqueue<coord> COORDQUEUE; // change this to circqueue if using yours.

// global matrix
set<coord> food;
int food_counter = 10;
char Matrix[mwidth][mheight] = {EMPTY};
char slayer_matrix[mwidth][mheight] = {EMPTY};
const int foods = 10; // number of food items;

struct dragon_base {   // don't change this class, INHERIT it.
  COORDQUEUE Q;
  sf::CircleShape circ;
  AnimatedGIF headgif; 
  sf::Vector2i headsize;
  sf::Sprite headsprite;

  dragon_base() : headgif{AnimatedGIF("head1.gif")} {}
  dragon_base(int x, int y) : headgif{AnimatedGIF("head1.gif")}
  {
    circ.setRadius(hgap/2.0f);
    circ.setFillColor(sf::Color::Green);
    headsize = headgif.getSize();
    headsprite.scale(1.25f*hgap/headsize.x,vgap*1.25f/headsize.y);
    Q.push_front({x,y});
    Q.push_front({x+1,y});
    Q.push_front({x+2,y});    
  } // constructor sets initial position, length 1

  void render(sf::RenderWindow& window) {
    int x, y;
    // draw only head as animated gif
    tie(x,y) = Q[0];
    headsprite.setPosition(x*hgap-headsize.x/20,y*vgap-headsize.y/20);
    headgif.update(headsprite);
    window.draw(headsprite);  
    for (int i=1;i<Q.len();i++) {//switched size to len
      tie(x,y) = Q[i];
      circ.setPosition(x*hgap,y*vgap);
      window.draw(circ);
    }
  }//render
};  // dragon_base class (can be inherited by subclass)

struct slayer_base {  // dragon slayer base class
  int x, y; // current position
  AnimatedGIF slayergif;
  sf::Vector2i slayersize;
  sf::Sprite slayersprite;

  slayer_base() : slayergif{AnimatedGIF("miner.gif")} {}
  slayer_base(int x0, int y0) : slayergif{AnimatedGIF("miner.gif")}
  {
    x = x0;  y = x0;
    slayersize = slayergif.getSize();
    slayersprite.scale(1.3f*hgap/slayersize.x,vgap*1.3f/slayersize.y);
    Matrix[x0][y0] ^= SLAYER;
  } // constructor

  void render(sf::RenderWindow& window) {
    slayersprite.setPosition(x*hgap-slayersize.x/20,y*vgap-slayersize.y/20);
    slayergif.update(slayersprite);
    window.draw(slayersprite);
  }
}; //slayer_base class


struct dragoon : dragon_base{
  dragoon(): dragon_base(0,0){
  }//default constructor
  dragoon(int x, int y): dragon_base(x,y){
  }//constructor

  void move(int dx, int dy){
    if (true){//change to not out of bounds
      int front_xpos = Q[0].first;
      int front_ypos = Q[0].second;
      int new_xpos = front_xpos + dx;
      int new_ypos = front_ypos + dy;
      pair<int,int> pop_pair;
      if ((new_xpos < 0) || (new_xpos > mwidth)){dx = 0;}//checks if out of bounds and doesnt move if it is
      if ((new_ypos < 0) || (new_ypos > mheight)){dy = 0;}

      if (Matrix[front_xpos + dx][front_ypos + dy] == FOOD){
        Q.push_front({front_xpos + dx, front_ypos +dy});
        Matrix[front_xpos + dx][front_ypos + dy] = EMPTY;
        food.erase({front_xpos + dx,front_ypos + dy});
        food_counter--;
      }
      else{
        Q.push_front({front_xpos + dx, front_ypos +dy});
        pop_pair = Q.pop_back();
        Matrix[pop_pair.first][pop_pair.second] = EMPTY;
      }
      Matrix[front_xpos + dx][front_ypos +dy] = DRAGON;
    }
  }//move

  bool getting_chopped(){
    bool has_chop = false;
    pair<int,int> Q_indexpair;
    for (int i = 0; i < Q.len(); i++){
      Q_indexpair = Q[i];
      if ((slayer_matrix[Q_indexpair.first][Q_indexpair.second] == SLAYER)|| has_chop){//slayer and dragon at tha cord
        has_chop = true;
        //cout<<has_chop<<endl;
        Matrix[Q_indexpair.first][Q_indexpair.second] = EMPTY;
        Q.remove_at(i);
      }
    }
    return has_chop;
  }//getting_chopped
};//DRAAGOOOOON

struct slaayeer : slayer_base{
  int randomstate;
  int euphoria;
  int sight;
  slaayeer(): slayer_base(mwidth,mheight){
    randomstate = 10;
    euphoria = 0;
    sight = 5;
  }//default constructor
  slaayeer(int x, int y): slayer_base(x,y){
    randomstate = 10;
    euphoria = 0;
    sight = 5;
  }//constructor

  int eucl_distance_between_two_coords(int dx, int dy){
    return sqrt(dx*dx + dy*dy);
  }//distance between 2 cords

  void move(int dragx, int dragy, bool has_been_chopped){//passed dragons mid x and y value
    int change_x = (rand()%3)-1;
    int change_y = (rand()%3)-1;
    int oldx = x;
    int oldy = y;
    int new_xpos = oldx + change_x;
    int new_ypos = oldy + change_y;

    bool insight;
    insight = (sight >= (eucl_distance_between_two_coords(dragx-oldx, dragy-oldy))); 
    
    if(euphoria <= 0 && has_been_chopped){
      euphoria = randomstate;
    }

    if ((new_xpos < 0) || (new_xpos > mwidth)){change_x = 0;}
    if ((new_ypos < 0) || (new_ypos > mheight)){change_y = 0;}//if out of bounds doesnt move

    if((euphoria > 0 || (!insight))){//also put if not out of bounds
      Matrix[change_x + x][change_y + y] ^= SLAYER;
      slayer_matrix[change_x + x][change_y + y] ^= SLAYER;

      Matrix[oldx][oldy] ^= SLAYER;
      slayer_matrix[oldx][oldy] ^= SLAYER;
      x = change_x + x;
      y = change_y + y;
      euphoria--;
    }
    else{ // follow the dragon cause its in sight, can move diag
      if (dragx > oldx){change_x = 1;}
      else if (dragx < oldx){change_x = -1;}
      else {change_x = 0;}

      if (dragy > oldy){change_y = 1;}
      else if (dragy < oldy){change_y = -1;}
      else {change_y = 0;}

      Matrix[change_x + x][change_y + y] ^= SLAYER;
      slayer_matrix[change_x + x][change_y + y] ^= SLAYER;
      Matrix[oldx][oldy] ^= SLAYER;
      slayer_matrix[oldx][oldy] ^= SLAYER;
      x = change_x + x;
      y = change_y + y;
    }
  }//move

};//slaayeer




int main(int argc, char** argv)
{
    using namespace sf;
    srand((unsigned int)time(0) % 10000); //seed random number generator
    RenderWindow window(VideoMode(wwidth, wheight), "Dragon");
    window.setFramerateLimit(4);
    
    Vertex hlines[2*mheight]; // 2 vertices for each line
    Vertex vlines[2*mwidth]; // 2 vertices for each line    

    auto color = Color(255,255,0,128); //::Yellow;
    for (int i=0; i < (2*mheight); i += 2) { // horizontal lines
        hlines[i] = Vertex(Vector2f(0.0f, i*vgap/2), color);
        hlines[i+1] = Vertex(Vector2f(wwidth-1, i*vgap/2), color);
    }
    for (int i=0; i < 2*mwidth; i += 2) { // vertical lines
        vlines[i] = Vertex(Vector2f(i*hgap/2, 0.0f), color);
        vlines[i+1] = Vertex(Vector2f(i*hgap/2, wheight-1), color);
    }

    dragoon dragon0(10,10);
    int dx = 0, dy = 0; // dragon movement vector
    slaayeer slayer0(25,35);

  //set<coord> food; // coordinates of food.
    int i = 0;  // set foods
    while (i<foods) {
      int x = rand()%mwidth;
      int y = rand()%mheight;
      if (Matrix[x][y]<DRAGON && Matrix[x][y]>=0) {
	      Matrix[x][y] = FOOD;
	      food.insert({x,y});
	      i++;
      }
    }//while


    CircleShape fcirc(hgap/4.0);       //graphical representation of food
    fcirc.setFillColor(Color::White);

    while (window.isOpen()&& (dragon0.Q.len() > 1))// && (food_counter!=0) && (dragon0.Q.len() > 1))// && (food_counter!=0)
    {
      sf::Event event;
      while (window.pollEvent(event))
      {
            if (event.type == sf::Event::Closed)
                window.close();
	    if (event.type==sf::Event::KeyPressed) {
	      if (event.key.code==sf::Keyboard::Up) { 
		      dy = -1;  dx = 0;
	      }
	      else if (event.key.code==sf::Keyboard::Down) { //down
		      dy = 1; dx=0;
	      }
	      else if (event.key.code==sf::Keyboard::Left) { //left
		      dx = -1; dy=0;
	      }
	      else if (event.key.code==72) { //right
		      dx=1; dy = 0;
	      }
	      else if (event.key.code==57) {  //stop
		      dx = dy = 0;
	      }
	    }// keyboard event handler
      }
        window.clear(sf::Color::Black); // erase background
        window.draw(vlines, 2*mwidth, sf::Lines); //draw lines
        window.draw(hlines, 2*mheight, sf::Lines); //draw lines
	for(auto& [x,y] : food) { // draw food items
	  fcirc.setPosition(x*hgap+hgap/4.0, y*vgap + vgap/4.0);
	  window.draw(fcirc);
	}
  // MOVE HERE
  dragon0.move(dx,dy);
  bool drag_chopped = dragon0.getting_chopped();
  pair<int,int> drag_mid = dragon0.Q[((dragon0.Q.len())/2)];
  int drag_midx =drag_mid.first;
  int drag_midy =drag_mid.second;
  slayer0.render(window);
  slayer0.move(drag_midx, drag_midy ,drag_chopped);


	dragon0.render(window);
	//slayer0.render(window);

  if (dragon0.Q.len() <= 0){
    cout<<"You lose, try to be better!"<<endl;
    break;
  }
  else if (food_counter <= 0){
    cout<<"You win!"<<endl;
    break;
  }

        window.display();
    }// main event loop

    return EXIT_SUCCESS;
}//main