#include <stdio.h>
#include <stdlib.h>
#define height 40
#define width 20
#define level1 20
#define level2 40
#define level3 60
#define level4 80
#define level5 100


/*Structure for all sort of objects - Food, mines & walls*/
struct obj
{
  int x,y;
};


/*Structure for the Snake*/
struct snake
{
  int x,y;
  char value;
  struct snake *next;
};


/*Function that generates the initial Snake*/
struct snake *initial_snake(struct snake *snake_head,int hash[width][height])
{
  int count;
  struct snake *node,*traverser;

  for(count = 0;count < 4;count++)
  {
    node = (struct snake*)malloc(sizeof(struct snake));
    if(count == 0)
    {
      node->value = 'O';
    }
    else if(count == 3)
    {
      node->value = 'x';
    }
    else
    {
      node->value = '*';
    }
    node->next = NULL;
    if(snake_head == NULL)
  	{
      node->x = 10;
      node->y = 10;
      hash[node->x][node->y] = 1;
      snake_head = node;
  	}
  	else
  	{
      traverser = snake_head;
      while(traverser->next)
      {
        traverser = traverser->next;
      }
      node->x = 10;
      node->y = snake_head->y - count;
      hash[node->x][node->y] = 1;
      traverser->next = node;
  	}
  }
  return snake_head;
}


/*Function that generates the initial Food*/
struct obj *initial_food(int hash[width][height])
{
  srand(time(0));
  struct obj *node;
  node = (struct obj*)malloc(sizeof(struct obj));
  node->x = rand() % (17 + 1 - 2) + 2;
  node->y = rand() % (37 + 1 - 2) + 2;
  if(hash[node->x][node->y] == 0)
    hash[node->x][node->y] = 2;
  else
    initial_food(hash);

  return node;
}


/*Function that generates the initial Board*/
void initial_board(char board[width][height])
{
  for(int row = 0;row < width;row++)
  {
    for(int col = 0;col < height;col++)
    {
      if(row == 0 || row == (width-1))  
      {
        board[row][col] = 'o';
      }
      else if(col == 0 || col == (height-1))
      {
        board[row][col] = '0';
      }
      else
      {
        board[row][col] = ' ';
      }
    }
  }
}


/*Function that checks for game over condition*/
int is_game_over(int x,int y,int hash[width][height])
{
  int game_over = 0;
  if(hash[x][y] == 1 || hash[x][y] == 3 || hash[x][y] == 9)
  {
    game_over = 1;
  }
  return game_over;
}


/*Function that generates the different objects: Food, walls and mines*/
struct obj *generate_obj()
{
  struct obj *poison = (struct obj *)malloc(sizeof(struct obj));
  poison->x = rand() % (17 + 1 - 2) + 2;
  poison->y = rand() % (37 + 1 - 2) + 2;
  return poison;
}


/*Function that updates food co-ordinates in the hash table*/
void update_food(int hash[width][height],struct obj *apple)
{ 
  apple = generate_obj(apple);
  if(hash[apple->x][apple->y] != 0 || apple->x == (width-1) || apple->x == 0 || apple->y == (height-1) || apple->y == 0)
  {
    apple = generate_obj(apple);
  }
  hash[apple->x][apple->y] = 2;
}

/*Function that updates snake co-ordinates in the hash table*/
void update_snake(char board[width][height],struct snake **snake_head)
{
  struct snake *snake_traverser = *snake_head;
  while(snake_traverser)
  {
    board[snake_traverser->x][snake_traverser->y] = snake_traverser->value;
    snake_traverser = snake_traverser->next;
  }
}


/*Function that increases the snake length upon eating food*/
void grow(struct snake **snake_head,int hash[width][height])
{
  struct snake *traverser = *snake_head,*added_node,*node;
  node = (struct snake*)malloc(sizeof(struct snake));
  node->value = 'x';
  while(traverser->next)
  {
    traverser = traverser->next;
  }
  node->x = traverser->x;
  node->y = traverser->y;
  traverser->value = '*';
  traverser->next = node;
}


/*Function that makes the snake move upon pressing: a,s,d,w*/
int move(struct snake **snake_head,int hash[width][height],char direction,struct obj *apple,char board[width][height])
{
    struct snake *traverser = *snake_head;
    int x_coord,y_coord,game_over = 0;
    int temp_x = traverser->x;
    int temp_y = traverser->y;
    traverser = traverser->next;

    char ch;
    switch(direction)
    {
      case 'w':
        game_over = is_game_over((*snake_head)->x-1,(*snake_head)->y,hash);
        if(game_over)
          return 1;
        (*snake_head)->x--;
        break;
      case 'a':
        game_over = is_game_over((*snake_head)->x,(*snake_head)->y-1,hash);
        if(game_over)
          return 1;
        (*snake_head)->y--;
        break;
      case 'd':
        game_over = is_game_over((*snake_head)->x,(*snake_head)->y+1,hash);
        if(game_over)
          return 1;
        (*snake_head)->y++;
        break;
      case 's':
        game_over = is_game_over((*snake_head)->x+1,(*snake_head)->y,hash);
        if(game_over)
          return 1;
        (*snake_head)->x++;
        break;
      default:
      {
        printf("Invalid key!\n");
        return 4;
      }
    }

    while(traverser)
    {
        x_coord = temp_x;
        y_coord = temp_y;
        temp_x = traverser->x;
        temp_y = traverser->y;
        traverser->x = x_coord;
        traverser->y = y_coord;
        traverser = traverser->next;
    }
    board[temp_x][temp_y] = ' ';
    hash[temp_x][temp_y] = 0;
  
    if(hash[(*snake_head)->x][(*snake_head)->y] == 2)
    {
      hash[(*snake_head)->x][(*snake_head)->y] = 1;
      update_food(hash,apple);
      grow(snake_head,hash);
      return 2;
    }

    if(!game_over)
        hash[(*snake_head)->x][(*snake_head)->y] = 1;


    return (game_over);
}



/*Function that makes the snake to pass through one boundary
 and enter through the opposite boundary*/
void boundary_pass(struct snake **snake_head,int hash[width][height]) 
{
  if((*snake_head)->x == 0) 
  {
    (*snake_head)->x = 18;
    hash[0][(*snake_head)->y] = 0;
    hash[18][(*snake_head)->y] = 1;
  }
  else if((*snake_head)->x == 19)
  {
    (*snake_head)->x = 1;
    hash[19][(*snake_head)->y] = 0;
    hash[1][(*snake_head)->y] = 1;
  }
  else if((*snake_head)->y == 0)
  {
    (*snake_head)->y = 38;
    hash[(*snake_head)->x][0] = 0;
    hash[(*snake_head)->x][38] = 1;
  }
  else if((*snake_head)->y == 39)
  {
    (*snake_head)->y = 1;
    hash[(*snake_head)->x][39] = 0;
    hash[(*snake_head)->x][1] = 1;
  }
}


/*Function to print game related instructions*/
void print_instructions(int score)
{
  printf("Generel Instructions: \n");
  printf("w : Move up\t| s : Move down\na : Move left\t| d : Move right\n");

  if(score < level1)
  {
    printf("\nLEVEL 1\n");
  }
  else if(score < level2)
  {
    printf("Mine kills the snake.\n");
    printf("\nLEVEL 2\n");
  }
  else if(score < level3)
  {
    printf("Mines kills the snake.\n");
    printf("\nLEVEL 3\n");
  }
  else if(score <= level4)
  {
    printf("Mines kills the snake.\nSnake can't pass through walls\n");
    printf("\nLEVEL 4\n");
  }

  printf("\n");
}


/*Function that displays the playing board*/
void display_board(struct snake **snake_head,struct obj *apple,char board[width][height],int hash[width][height],int score)
{
  printf("~~~~~ APPLE & THE SNAKE ~~~~~\n\n");
  
  print_instructions(score);

  update_snake(board,snake_head);

  for(int row = 0;row < width;row++)
  {
    for(int col = 0;col < height;col++)
    {
      if(hash[row][col] == 3)
        printf("$");
      else if(hash[row][col] == 2)
        printf("@");
      else if(hash[row][col] == 9)
        printf("+");
      else
        printf("%c",board[row][col]);
    }
    printf("\n");
  }
  printf("Score: %d\n",score);
}


/*Function that handles direction inputs and score*/
int play(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height],int level_score)
{
  int game_over = 0;
  static int score = 0;
  char direction;

  do
  {
    scanf("%c",&direction);
    while(getchar() != '\n');
      game_over = move(snake_head,hash,direction,apple,board);

    if(game_over == 2)
      score++;

    boundary_pass(snake_head,hash);
    system("clear");
    display_board(snake_head,apple,board,hash,score);
  }
  while(game_over != 1 && score != level_score);

  return game_over;
}

/*Function that updates mine co-ordinates to the hash*/
void update_mine(int hash[width][height])
{ 
  struct obj *poison = generate_obj();
  if(hash[poison->x][poison->y] != 0 || poison->x == (width-1) || poison->x == 0 || poison->y == (height-1) || poison->y == 0)
  {
    poison = generate_obj();
  }

  hash[poison->x][poison->y] = 3;
}


/*Function that creates boundary in level 4*/
void create_boundary(int hash[width][height])
{
  for(int row = 0;row < width;row++)
  {
    for(int col = 0;col < height;col++)
    {
      if(row == 0 || row == (width-1) || col == 0 || col == (height-1))  
      {
        hash[row][col] = 1;
      }
    }
  }
}


/*Function that updates wall co-ordinates to the hash*/
void update_wall(int hash[width][height])
{
  struct obj *wall_1,*wall_2;

  wall_1 = generate_obj();
  if(hash[wall_1->x][wall_1->y] != 0 || wall_1->x == (width-1) || wall_1->x == 0 || wall_1->y == (height-1) || wall_1->y == 0)
  {
    wall_1 = generate_obj();
  }

  for(int brick = 0; brick < 5; brick++)
  {
    if(hash[wall_1->x+1][wall_1->y] != 0)
      break;

    hash[wall_1->x++][wall_1->y] = 9;
  }

  wall_2 = generate_obj();
  if(hash[wall_2->x][wall_2->y] != 0 || wall_2->x == (width-1) || wall_2->x == 0 || wall_2->y == (height-1) || wall_2->y == 0)
  {
    wall_2 = generate_obj();
  }

  for(int brick = 0; brick < 4; brick++)
  {
    if(hash[wall_2->x][wall_2->y+1] != 0)
      break;

    hash[wall_2->x][wall_2->y++] = 9;
  }
}


/*Function that handles game in level 5*/
void level_5(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height])
{ 
  update_wall(hash);
  int state = play(snake_head,apple,hash,board,level5);
  
  if(state == 1)
    return;
   else if(state != 1)
    printf("You Win!\n");
}


/*Function that handles game in level 4*/
void level_4(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height])
{ 
  create_boundary(hash);

  int state = play(snake_head,apple,hash,board,level4);

  if(state == 1)
    printf("You lost!\n");
  if(state != 1)
    level_5(snake_head,apple,hash,board);
}


/*Function that handles game in level 3*/
void level_3(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height])
{ 
  for(int obj_count = 0;obj_count < 3;obj_count++)
    update_mine(hash);

  int state = play(snake_head,apple,hash,board,level3);
  if(state == 1)
    printf("You lost!\n");
  else if(state != 1)
    level_4(snake_head,apple,hash,board);
}


/*Function that handles game in level 2*/
void level_2(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height])
{ 
  update_mine(hash);
  int state = play(snake_head,apple,hash,board,level2);
  if(state == 1)
    printf("You lost!\n");
  else if(state != 1)
    level_3(snake_head,apple,hash,board);
}


/*Function that handles game in level 1*/
void level_1(struct snake **snake_head,struct obj *apple,int hash[width][height],char board[width][height])
{
  int state = play(snake_head,apple,hash,board,level1);

  if(state == 1)
    printf("You lost!\n");
  else if(state != 1)
    level_2(snake_head,apple,hash,board);
}


int main()
{
  system("clear");
  
  struct snake *snake_head = NULL;
  struct obj *apple = NULL; 
  char board[width][height];
  int hash[width][height] = {0},initial_score = 0;

  snake_head = initial_snake(snake_head,hash);
  apple = initial_food(hash);
  initial_board(board);
  display_board(&snake_head,apple,board,hash,initial_score);
  level_1(&snake_head,apple,hash,board);
}