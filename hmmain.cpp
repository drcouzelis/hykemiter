

#include "hminc.h"


int time_check (int time1, int time2);


PALLETE game_pal;
volatile int timer;

void do_timer (void)
{
  timer+=1;
  if (timer >= MAX_TICKER)
     timer = 1;
}
END_OF_FUNCTION (do_timer);



typedef enum  {
   OFF=0, ON=1
}  FLAG;


typedef enum  {
   U=1,D=2,L=3,R=4
}  DIR;


typedef enum  {
   stand=0, run=1, jump=2, fall=3
}  ANIM_TYPE;


struct MOVEMENT  {
   int size;
   double fudge;
   int max;
};



class GROUND  {

   BITMAP *pic;

   int ref_x_pos;
   int change_x;
   int x_offset;

   public:
      GROUND (BITMAP *pic_inc, int start_x, int change_x_inc);
      ~GROUND (void);

      void move (int x, int y);
      void draw (BITMAP *bmp_screen, int x_loc, int y_loc);

      void print_info (void);

};



//THIS FUNCTION IS NOT FINISHED!!
void GROUND::draw (BITMAP *bmp_screen, int x_loc, int y_loc)
{
   //masked_blit (pic, bmp_screen, x_offset,0, 0,0, 320,200);
}


//THIS FUNCTION IS NOT FINISHED!!
void GROUND::move (int x, int y)
{
   //AKA, 'update', with reference to pos x,y.
   /*
   if (abs (x-ref_x_pos) >= change_x)
   {
      x_offset += (x-ref_x_pos)/change_x;
      ref_x_pos = x - ((x-ref_x_pos)%3);      
   }
   if (abs (y-ref_y_pos) >= change_y)
   {
      y_offset += (y-ref_y_pos)/change_y;
      ref_y_pos = y - ((y-ref_y_pos)%3);      
   }
   */
}


GROUND::~GROUND (void)
{
   clear (pic);
   destroy_bitmap (pic);
}


void GROUND::print_info (void)
{
   masked_blit (pic, screen, 0,0, 0,0, 320,200);
   readkey ();
}


GROUND::GROUND (BITMAP *pic_inc, int start_x, int change_x_inc)
{
   pic = create_bitmap (pic_inc->w, pic_inc->h);
   clear (pic);
   blit (pic_inc, pic, 0,0, 0,0, pic_inc->w,pic_inc->h);

   ref_x_pos = start_x;
   x_offset = 0;
   change_x = change_x_inc;
}


class ANIM_SET  {

   int size;     //Num of frames.
   int curr;     //Current frame you're on.
   FLAG loop;    //Whether the anim loops or not.

   int speed;    //How fast the anim runs.
   int time_reference;

   int hit_rad_h, hit_rad_v;    //The h and v radii of the anim set.
   BITMAP **pic;                //The array of pics.

   public:
      ANIM_SET (BITMAP *pic_inc, int hit_r_h, int hit_r_v, int size_inc, int speed_inc, FLAG loop_inc);
      ~ANIM_SET (void);

      void animate (void);
      void reset (void)  {  curr = 0;  time_reference = timer;  }
      BITMAP * get_pic (void)  {  return (pic[curr]);  }
      int get_w (void)  {  return (pic[curr]->w);  }
      int get_h (void)  {  return (pic[curr]->h);  }

      int call_hit_rh (void)  {  return (hit_rad_h);  }
      int call_hit_rv (void)  {  return (hit_rad_v);  }

};



void ANIM_SET::animate (void)
{

   if (time_check (timer, time_reference) >= speed)
   {
      time_reference = timer;
      if (curr < size-1)
         curr += 1;
      else
         if (loop == ON)
            curr = 0;
   }
}


ANIM_SET::ANIM_SET (BITMAP *pic_inc, int hit_r_h, int hit_r_v, int size_inc, int speed_inc, FLAG loop_inc)
{

   size = size_inc;
   loop = loop_inc;

   speed = speed_inc;
   time_reference = timer;

   hit_rad_h = hit_r_h;
   hit_rad_v = hit_r_v;

   pic = new BITMAP * [size];
   for (int j=0; j<size; j+=1)
      pic[j] = NULL;

   for (int i=0; i < size; i += 1)
   {
      pic[i] = create_bitmap (pic_inc->w/size, pic_inc->h);
      clear (pic[i]);
      masked_blit (pic_inc, pic[i], (pic_inc->w/size)*i,0, 0,0, pic_inc->w/size,pic_inc->h);
   }

   curr = 0;

}


ANIM_SET::~ANIM_SET (void)
{
   for (int i=0; i<size; i+=1)
   {
      //printf ("checking pic %d... \n", i);  readkey ();
      if (pic[i])
      {
         clear (pic[i]);
         destroy_bitmap (pic[i]);
         //printf ("pic %d deleted \n", i);  readkey ();
      }
   }

   delete pic;
}


class LEVEL {
   
   BITMAP *farground;
   GROUND *scrollground;
   BITMAP *walkground;

   int start_x, start_y;
   char next_level[20];

   //int end_of_level_x;

   public:
      LEVEL (char file_name[20]);
      ~LEVEL (void);

      int call_start_x (void)  {  return (start_x);  }
      int call_start_y (void)  {  return (start_y);  }
      BITMAP * call_walkground (void)  {  return (walkground);  }

      void draw (BITMAP *pic_screen, int around_x, int around_y);

      void print_info (void);
      
      //scroll (DIR dir, int change);

};


void LEVEL::draw (BITMAP *pic_screen, int around_x, int around_y)
{
   //First the farground...
   blit (farground, pic_screen, 0,0, 0,0, 320,200);
   //...Next the scrollground...

   //...Finally the walkground!
   masked_blit (walkground, pic_screen, (around_x-160),(around_y-100), 0,0, 320,200);
   //masked_blit (walkground, screen, 195,1535, 0,0, 320,200);
}


LEVEL::~LEVEL (void)
{
   clear (farground);
   clear (walkground);
   destroy_bitmap (farground);
   destroy_bitmap (walkground);

   delete scrollground;
}


void LEVEL::print_info (void)
{
   blit (farground, screen, 0,0, 0,0, 320,200);
   readkey ();
   scrollground->print_info ();
}


LEVEL::LEVEL (char file_name[20])
{

   FILE *mary;
   char str[250];
   char pic_file_name[50];


   if ((mary = fopen (file_name, "r")) == NULL)
      printf ("level not initialized \n");

   else
   {
      while ((fscanf (mary, "%s", str)) != EOF)
      {

         if ((strcmp ("start_x", str)) == 0)
            fscanf (mary, "%d", &start_x);
         if ((strcmp ("start_y", str)) == 0)
            fscanf (mary, "%d", &start_y);

         if ((strcmp ("walkground", str)) == 0)
         {
            int num1, num2;

            fscanf (mary, "%s", pic_file_name);
            fscanf (mary, "%d", &num1);
            fscanf (mary, "%d", &num2);

            walkground = create_bitmap (num1, num2);
            clear (walkground);
            walkground = load_bitmap (pic_file_name, game_pal);
            //blit (walkground, screen, 195,1535, 0,0, 320,200);  readkey ();

         }

         if ((strcmp ("farground", str)) == 0)
         {
            int num1, num2;

            fscanf (mary, "%s", pic_file_name);
            fscanf (mary, "%d", &num1);
            fscanf (mary, "%d", &num2);

            farground = create_bitmap (num1, num2);
            clear (farground);
            farground = load_bitmap (pic_file_name, game_pal);
         }

         if ((strcmp ("scrollground", str)) == 0)
         {
            int num1, num2;
            int start_x;
            int change_x_inc;
            BITMAP *pic_temp;

            fscanf (mary, "%s", pic_file_name);
            fscanf (mary, "%d", &num1);
            fscanf (mary, "%d", &num2);
            //fscanf (mary, "%d", &start_x);
            //fscanf (mary, "%d", &start_y);
            fscanf (mary, "%d", &change_x_inc);
            //fscanf (mary, "%d", &change_y_inc);

            pic_temp = create_bitmap (num1, num2);
            clear (pic_temp);
            pic_temp = load_bitmap (pic_file_name, game_pal);

            scrollground = new GROUND (pic_temp, (start_x-160), change_x_inc);
         }

         if ((strcmp ("next_level", str)) == 0)
            fscanf (mary, "%s", next_level);

      }
   }
   fclose (mary);

}



class SPRITE  {

   int health;
   DIR facing;
   
   int x_pos, y_pos;

   MOVEMENT h_vel, v_vel;
   MOVEMENT h_acc, v_acc;

   int hit_x, hit_y;
   int hit_w, hit_h;

   ANIM_SET *anim[10];
   ANIM_TYPE curr_anim;

   int jump_height;

   //Send change in values and BMP to refer to (level), return true if successful.
   //FLAG move (int change_x, int change_y, BITMAP *pic);
   FLAG pass_l (BITMAP *pic);
   FLAG pass_r (BITMAP *pic);
   FLAG pass_u (BITMAP *pic);
   FLAG pass_d (BITMAP *pic);
      

   void update_vel_h (BITMAP *reference, int velocity);
   void update_vel_v (BITMAP *reference, int velocity);
   void update_acc_h (int accel);
   void update_acc_v (int accel);


   public:
      int call_x_pos (void)  {  return (x_pos);  }
      int call_y_pos (void)  {  return (y_pos);  }

      BITMAP * frame_pic (ANIM_TYPE type)  {  return (anim[type]->get_pic());  }
      int frame_w (ANIM_TYPE type)  {  return (anim[type]->get_w());  }
      int frame_h (ANIM_TYPE type)  {  return (anim[type]->get_h());  }
      void animate (ANIM_TYPE type)  {  return (anim[type]->animate());  }

      FLAG pass_check (BITMAP *pic, DIR dir);

      void print_info (BITMAP *pic_screen);

      //Constructor:: send the file with the info on the sprite.
      SPRITE (char file_name[20]);
      ~SPRITE (void);

      void init_to_level (LEVEL *level);
      void draw (BITMAP *pic_screen, BITMAP *reference);

      void change_health (int change)  {  health += change;  }
      void change_facing (DIR new_dir)  {  facing = new_dir;  }

      void add_gravity (BITMAP *reference);
      void check_input (BITMAP *reference);
      void update (BITMAP *reference, BITMAP *pic_screen);

      void change_acc_h (int accel)  {  h_acc.size = accel;  }
      void change_vel_v (int vel)  {  v_vel.size = vel;  }


      //duck ();
      //jump (int height);
      //run (DIR dir);

};


void SPRITE::update_acc_h (int accel)
{
   h_acc.size = accel;
   if (h_acc.size > h_acc.max)
      h_acc.size = h_acc.max;
   if (h_acc.size < -h_acc.max)
      h_acc.size = -h_acc.max;

   h_vel.size += h_acc.size/60;
   h_acc.fudge += ((float)h_acc.size / (float)60) - (h_acc.size/60);

   while (h_acc.fudge > 1)
   {
      h_acc.fudge -= 1;
      h_vel.size += 1;
   }
   while (h_acc.fudge < -1)
   {
      h_acc.fudge += 1;
      h_vel.size -= 1;
   }

}


void SPRITE::update_acc_v (int accel)
{
   v_acc.size = accel;
   if (v_acc.size > v_acc.max)
      v_acc.size = v_acc.max;
   if (v_acc.size < -v_acc.max)
      v_acc.size = -v_acc.max;

   v_vel.size += v_acc.size/60;
   v_acc.fudge += ((float)v_acc.size / (float)60) - (v_acc.size/60);

   while (v_acc.fudge > 1)
   {
      v_acc.fudge -= 1;
      v_vel.size += 1;
   }
   while (v_acc.fudge < -1)
   {
      v_acc.fudge += 1;
      v_vel.size -= 1;
   }

}




void SPRITE::update_vel_h (BITMAP *reference, int velocity)
{

   h_vel.size = velocity;
   if (h_vel.size > h_vel.max)
      h_vel.size = h_vel.max;
   if (h_vel.size < -h_vel.max)
      h_vel.size = -h_vel.max;

   int move = h_vel.size/60;
   h_vel.fudge += ((float)h_vel.size / (float)60) - (h_vel.size/60);

   //Don't forget to take into account + and -...
   //This adjusts the fudge factor
   while (h_vel.fudge > 1)
   {
      h_vel.fudge -= 1;
      move += 1;
   }
   while (h_vel.fudge < -1)
   {
      h_vel.fudge += 1;
      move -= 1;
   }

   //Move the character 'move' spaces...
   int i;
   FLAG pass = ON;

   while (move > 0 && pass)
   {
      //x_pos += 1;

      change_facing (R);
      for (i=0; !(pass = pass_check (reference, R)) && i<=10; i+=1)
         y_pos -= 1;
      if (pass)  x_pos += 1;
      else  y_pos += i;

      move -= 1;

   }
   while (move < 0 && pass)
   {
      //x_pos -= 1;

      change_facing (L);
      for (i=0; !(pass = pass_check (reference, L)) && i<=10; i+=1)
         y_pos -= 1;
      if (pass)  x_pos -= 1;
      else  y_pos += i;

      move += 1;

   }


}


void SPRITE::update_vel_v (BITMAP *reference, int velocity)
{

   v_vel.size = velocity;
   if (v_vel.size > v_vel.max)
      v_vel.size = v_vel.max;
   if (v_vel.size < -v_vel.max)
      v_vel.size = -v_vel.max;

   int move = v_vel.size/60;
   v_vel.fudge += ((float)v_vel.size / (float)60) - (v_vel.size/60);

   //Don't forget to take into account + and -...
   //This adjusts the fudge factor
   while (v_vel.fudge > 1)
   {
      v_vel.fudge -= 1;
      move += 1;
   }
   while (v_vel.fudge < -1)
   {
      v_vel.fudge += 1;
      move -= 1;
   }

   //Move the character 'move' spaces...
   //This method takes care of vertical movement
   while (move > 0 && pass_check (reference, D))
   {
      y_pos += 1;
      move -= 1;
   }
   while (move < 0 && pass_check (reference, U))
   {
      y_pos -= 1;
      move += 1;
   }
}


void SPRITE::init_to_level (LEVEL *level)
{
   x_pos = level->call_start_x();
   y_pos = level->call_start_y();
}


void SPRITE::update (BITMAP *reference, BITMAP *pic_screen)
{
   //This doesn't really need to be sent 'reference'
   check_input (reference);

   update_acc_v (GRAVITY);
   update_vel_v (reference, v_vel.size);

   update_acc_h (h_acc.size);
   update_vel_h (reference, h_vel.size);
   //add_gravity (reference);

   if (!pass_check (reference, D))
      v_vel.size = 0;

   anim[curr_anim]->animate();
   draw (pic_screen, reference);

}


void SPRITE::check_input (BITMAP *reference)
{
   if (key[KEY_RIGHT])
   {
      change_acc_h (1500);
      curr_anim = run;

      /*
      int i;
      FLAG pass;
      change_facing (R);
      for (i=0; !(pass = pass_check (reference, R)) && i<=10; i+=1)
         y_pos -= 1;
      if (pass)  x_pos += 1;
      else  y_pos += i;
      */
   }

   else if (key[KEY_LEFT])
   {
      change_acc_h (-1500);
      curr_anim = run;

      /*
      int i;
      FLAG pass;
      change_facing (L);
      for (i=0; !(pass = pass_check (reference, L)) && i<=10; i+=1)
         y_pos -= 1;
      if (pass)  x_pos -= 1;
      else  y_pos += i;
      */
   }

   else
   {
      change_acc_h (0);
      h_vel.size = 0;
      curr_anim = stand;

      /*
      if (h_vel.size > 0)
         change_acc_h (-800);
      if (h_vel.size < 0)
         change_acc_h (800);
      if (h_vel.size <= 1 || h_vel.size >= -1)
      {
         change_acc_h (0);
         h_vel.size = 0;
      }
      */

   }


   if (!pass_check (reference, D))
      if (key[KEY_Z])
         change_vel_v (-500);


}


void SPRITE::add_gravity (BITMAP *reference)
{
   //A simple gravity function.
   if (pass_check (reference, D))
      y_pos += 1;
}



void SPRITE::draw (BITMAP *pic_screen, BITMAP *reference)
{

   //Use reference later to know where to blit on the screen...
   if (facing == R)
      draw_sprite (pic_screen, frame_pic(curr_anim), (160-(frame_w(stand)/2)), (100-(frame_h(stand)/2)));
   else  draw_sprite_h_flip (pic_screen, frame_pic(curr_anim), (160-(frame_w(stand)/2)), (100-(frame_h(stand)/2)));
}



FLAG SPRITE::pass_check (BITMAP *pic, DIR dir)
{
   if (dir == U)
      return (pass_u (pic));
   else if (dir == D)
      return (pass_d (pic));
   else if (dir == L)
      return (pass_l (pic));
   else if (dir == R)
      return (pass_r (pic));
   else  return (ON);
}


FLAG SPRITE::pass_l (BITMAP *pic)
{
   //Only need to check every other pixel, at most.
   int y_temp = y_pos - (anim[curr_anim]->call_hit_rv());
   int y_final = y_pos + (anim[curr_anim]->call_hit_rv());
   int x_line = x_pos - (anim[curr_anim]->call_hit_rh()) - 1;

   for ( ; y_temp <= y_final; y_temp+=2)
   {
      if (getpixel (pic, x_line, y_temp) == 185)  //color black
         return (OFF);
   }
   return (ON);
}


FLAG SPRITE::pass_r (BITMAP *pic)
{
   int y_temp = y_pos - (anim[curr_anim]->call_hit_rv());
   int y_final = y_pos + (anim[curr_anim]->call_hit_rv());
   int x_line = x_pos + (anim[curr_anim]->call_hit_rh()) + 1;

   for ( ; y_temp <= y_final; y_temp+=2)
   {
      if (getpixel (pic, x_line, y_temp) == 185)  //color black
         return (OFF);
   }
   return (ON);
}


FLAG SPRITE::pass_u (BITMAP *pic)
{
   int x_temp = x_pos - (anim[curr_anim]->call_hit_rh());
   int x_final = x_pos + (anim[curr_anim]->call_hit_rh());
   int y_line = y_pos - (anim[curr_anim]->call_hit_rv()) - 1;

   for ( ; x_temp <= x_final; x_temp+=2)
   {
      if (getpixel (pic, x_temp, y_line) == 185)  //color black
         return (OFF);
   }
   return (ON);
}


FLAG SPRITE::pass_d (BITMAP *pic)
{
   int x_temp = x_pos - (anim[curr_anim]->call_hit_rh());
   int x_final = x_pos + (anim[curr_anim]->call_hit_rh());
   int y_line = y_pos + (anim[curr_anim]->call_hit_rv()) + 1;

   for ( ; x_temp <= x_final; x_temp+=2)
   {
      if (getpixel (pic, x_temp, y_line) == 185)  //color black
         return (OFF);
   }
   return (ON);
}



SPRITE::~SPRITE (void)
{
   for (int i=0; i<10; i+=1)
   {
      //printf ("checking anim %d... \n", i);  readkey ();
      if (anim[i])
      {
         delete anim[i];
         //printf ("deleted anim %d \n", i);  readkey ();
      }
   }

   //printf ("deleting anim... \n");  readkey ();
   //delete anim;
   //printf ("deleted!  \n");  readkey ();
}


void SPRITE::print_info (BITMAP *pic_screen)
{


   textprintf_ex (pic_screen, font, 5,0, 1, -1, "CHARACTER");

   textprintf_ex (pic_screen, font, 5,20, 1, -1, "h_acc size %d fudge %f", h_acc.size, h_acc.fudge);
   textprintf_ex (pic_screen, font, 5,30, 1, -1, "v_acc size %d fudge %f", v_acc.size, v_acc.fudge);

   textprintf_ex (pic_screen, font, 5,40, 1, -1, "h_vel size %d fudge %f", h_vel.size, h_vel.fudge);
   textprintf_ex (pic_screen, font, 5,50, 1, -1, "v_vel size %d fudge %f", v_vel.size, v_vel.fudge);


   /*
   printf ("health = %d \n", health);
   printf ("dir = %d \n", facing);
   printf ("pos x = %d \npos y = %d \n", x_pos, y_pos);
   //printf ("momentum x = %d \nmomentum y = %d \n", x_momentum, y_momentum);
   masked_blit (frame_pic(stand), screen, 0,0, 320-frame_w(stand),0, frame_w(stand),frame_h(stand));
   */
}


SPRITE::SPRITE (char file_name[20])
{

   FILE *mary;
   char str[400];
   char pic_file_name[50];

   facing = R;
   x_pos = 0;
   y_pos = 0;

   h_vel.size=0;  h_vel.fudge=0;  h_vel.max=200;
   v_vel.size=0;  v_vel.fudge=0;  v_vel.max=5000;
   h_acc.size=0; h_acc.fudge=0;  h_acc.max=10000;
   v_acc.size=GRAVITY;  v_acc.fudge=0;  v_acc.max=10000;


   curr_anim = stand;


   for (int j=0; j<10; j+=1)
      anim[j] = NULL;

   if ((mary = fopen (file_name, "r")) == NULL)
      printf ("sprite not initialized \n");

   else
   {
      while ((fscanf (mary, "%s", str)) != EOF)
      {

         if ((strcmp ("health", str)) == 0)
            fscanf (mary, "%d", &health);

         /*
         if ((strcmp ("hit_detection", str)) == 0)
         {
            fscanf (mary, "%d", &hit_x);
            fscanf (mary, "%d", &hit_y);
            fscanf (mary, "%d", &hit_w);
            fscanf (mary, "%d", &hit_h);
         }
         */

         if ((strcmp ("pic_file", str)) == 0)
            if ((fscanf (mary, "%s", pic_file_name)) == NULL)
               printf ("no pic file loaded \n");

         if ((strcmp ("anim_sets", str)) == 0)
         {
            ANIM_TYPE type;
            int anim_pic_pos;
            int num_of_frames;
            int loop_flag;
            int time_to_wait;
            int frame_w, frame_h;
            int hit_r_h, hit_r_v;
            BITMAP *bmp_whole;
            BITMAP *bmp_part;

            bmp_whole = create_bitmap (500, 500);
            clear (bmp_whole);
            bmp_whole = load_bitmap (pic_file_name, game_pal);


            fscanf (mary, "%s", str);
            while ((strcmp ("end_anim_sets", str)) != 0)
            {
               if ((strcmp ("stand", str)) == 0)
                  type = stand;
               if ((strcmp ("run", str)) == 0)
                  type = run;
               if ((strcmp ("jump", str)) == 0)
                  type = jump;
               if ((strcmp ("fall", str)) == 0)
                  type = fall;

               fscanf (mary, "%d", &anim_pic_pos);
               fscanf (mary, "%d", &frame_w);
               fscanf (mary, "%d", &frame_h);
               fscanf (mary, "%d", &hit_r_h);
               fscanf (mary, "%d", &hit_r_v);
               fscanf (mary, "%d", &num_of_frames);
               fscanf (mary, "%d", &time_to_wait);
               fscanf (mary, "%d", &loop_flag);

               bmp_part = create_bitmap (frame_w * num_of_frames, frame_h);
               clear (bmp_part);
               blit (bmp_whole, bmp_part, 0,(anim_pic_pos-1)*frame_h, 0,0, frame_w*num_of_frames,frame_h);

               anim[type] = new ANIM_SET (bmp_part, hit_r_h, hit_r_v, num_of_frames, time_to_wait, (FLAG) loop_flag);
               //printf ("anim set #%d initialized \n", type);  readkey ();

               fscanf (mary, "%s", str);
            }


         }

      }
   }
   fclose (mary);

}



int main (void)
{

   FLAG game_over = OFF;
   BITMAP *screen_buffer;

   allegro_init ();
   install_keyboard ();
   set_color_depth (8);
   set_gfx_mode (GFX_AUTODETECT_WINDOWED, 320, 200, 0, 0);

   install_timer ();
   install_int_ex(do_timer,BPS_TO_TIMER(GAME_TICKER));
   LOCK_VARIABLE (timer);
   LOCK_FUNCTION (do_timer);
   timer=0;

   screen_buffer = create_bitmap (320,200);
   clear (screen_buffer);


   //This is ok for now, but only initialize the cc when I
   //need it.....  (main game loop start)
   //SPRITE cc ("hykedata.txt");
   SPRITE *cc;
   LEVEL *level;

   printf ("creating new sprite... \n");  //readkey ();
   cc = new SPRITE ("hykedata.txt");
   printf ("creating new level... \n");  //readkey ();
   level = new LEVEL ("level1.txt");
   printf ("level and cc set. \n");  //readkey ();

   cc->init_to_level (level);
   //transparent color == 185
   set_pallete (game_pal);


   clear_to_color (screen, 215);
   //level->print_info();  readkey ();
   //printf ("level printed \n");  readkey ();
   //delete level;
   //printf ("level deleted \n");  readkey ();

   //cc->print_info ();  readkey ();

   int game_rest = 0;

   while (!game_over)
   {
      level->draw (screen_buffer, cc->call_x_pos(), cc->call_y_pos());

      cc->update (level->call_walkground(), screen_buffer);
      cc->print_info (screen_buffer);

      vsync ();
      blit (screen_buffer, screen, 0,0, 0,0, 320,200);

      if (key[KEY_ESC])
         game_over = ON;

      while (timer - game_rest < 2);
      game_rest = timer;
   }

   readkey ();

   delete cc;
   //printf ("cc deleted, and being re installed! \n");  readkey ();
   //cc = new SPRITE ("hykedata.txt");

   //clear_to_color (screen, 215);
   //cc->print_info ();

   //readkey ();
   return (1);

}
END_OF_MAIN()


int time_check (int time1, int time2)
{
   //returns time 1 (a later time) minus time 2 (the earlier time).
   if (time1 - time2 >= 0)
      return (time1 - time2);
   return (time1 - (time2 - MAX_TICKER));
}








