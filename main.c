/*
Author: Nickmeny
Ο παρακάτω κώδικας ειναι για το jump mechanics. Υλοποιει τους τυπους που εξηγησαμε.
Η χρηση raylib ειναι απλα για να δειξουμε το mechanic. Μπορει να εφαρμοστει σε οποια αλλη γλωσσα 
και σε οτι αλλο engine θελουμε.
Κατι για τους φιλους μας τους προτωετεις ή για οσους δεν εχουν καλη "σχεση" με την c ή γενικα με τον 
προγραμματισμο.
Τα structs ειναι δομες οπου τις δηλωνουμε εμεις και εχουν αλλους τυπους μεσα τους.
πχ Player που εχει την ταχυτητα του το position του κτλ. 
το typedef ειναι μια εντολη στην οποια δινουμε δικα μας ονοματα (και οχι μονο) σε τυπους δεδομενων
δικους μας ή και οχι πχ typedef int myint μπορουμε να γραφουμε myint x = 3 
ειναι σαν να εχουμε int x =3.
Το #define ειναι απλα οτι ο preprocessor οπου βλεπει το ονομα που ακολουθει το αντικαθησταει με την τιμη μετα

ΓΕΝΙΚΑ ΜΗΝ ΔΩΣΕΤΕ ΒΑΣΗ ΣΕ ΑΥΤΑ, ΘΑ ΤΑ ΜΑΘΕΤΕ, ΑΥΤΟ ΠΟΥ ΠΡΕΠΕΙ ΝΑ ΔΩΣΕΤΕ ΒΑΣΗ ΕΙΝΑΙ Η ΕΦΑΡΜΟΓΗ ΑΠΟ ΤΗΝ ΘΕΩΡΙΑ

Τελος μια μικρη διευκρινηση για τους αξωνες. Στην raylib και γενικα στα  graphics lib το 0,0 ειναι σχεδον παντα
στην πανω αριστερη γωνια του παραθυρου, αρα οσο κατεβαινεις αυξανεται το y  
*/

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_TRAIL_POINTS 300 //Για να εμφανιζουμε τις γραμμες της παραβολης 

//JumpConfig ειναι ενα στρακτ που κραταει ολες τις μεταβλητες που θελουμε για τις συναρτησεις
//εκτος απο την v_x επειδη "λογικα" ειναι attribute του παιχτη
typedef struct {
    float h; //το max υψος που θελουμε να εχει το jump 
    float x_h; //η αποσταση απο το x0 μεχρι το xh δηαλδη να φτασει στην θεση οπου max h
    float jump_force; // το "διανυσμα"/δυναμη που ασκηται
    float gravity;  //η δυναμη της βαρυτητας
}JumpConfig;

typedef struct
{
    Vector2 pos; //το position του παιχτη ( x,y), το vector2 ειναι ενα struct που μας δινει η raylib και εχει μεσα δυο πεδια τα float x κια float y
    Vector2 velocity; //η ταχυτητα του παιχτη
    const float v_x; // η ταχυτηρα που κινηται ο παιχτης στον χ'χ ( foot speed)
    bool is_grounded; // για να δουμε αν μπορει να εκτελεσει το jump 
    JumpConfig jump_conf; //ενας πινακας μεγεθους 2 απο JumpConfigs ( χρειαζεται για το double jump, αν δεν ειχαμε double jump και απλα ενα πεδιο JumpConfig jump_conf θα ηταν μια χαρα)
}Player;

/*
ΑΠΟ ΕΔΩ ΜΕΧΡΙ ΚΑΙ ΛΙΓΟ ΠΙΟ ΜΕΤΑ ΕΙΝΑΙ ΕΝΑ ΠΕΡΙΕΡΓΟ ΚΟΜΜΑΤΙ. ΕΙΝΑΙ ΑΠΛΑ ΓΙΑ ΝΑ ΖΩΓΡΑΦΙΖΟΥΜΕ ΤΙΣ ΚΑΜΠΥΛΕΣ
ΑΝ ΔΕΝ ΘΕΛΕΤΕ ΜΗΝ ΤΟΥ ΔΩΣΕΤΕ ΣΗΜΑΣΙΑ ΑΛΑΛ ΕΙΝΑΙ ΕΝΔΙΑΦΕΡΟΝ ΓΙΑ ΟΠΟΙΟΝ ΕΧΕΙ ΟΡΕΞΗ
*/

Vector2 trailPoints[MAX_TRAIL_POINTS]; // Πινακας απο x,y για συντεταγμενες των σημειων
int trailCount = 0; //το index του πινακα

//Βοιηθητκη συναρτηση για να ειδαγουμε ενα point στον πινακα
void AddTrailPoint(Vector2 pos) {
    if (trailCount < MAX_TRAIL_POINTS) {
        trailPoints[trailCount] = pos;
        trailCount++;
    }
}
//Απλα κανουμε reset το index
void ClearTrail(void) {
    trailCount = 0;
}

// Ζωγραφίζει την  τροχια που εχει διαχησει ο παιχτης μεχρι τωρα
void DrawRecordedTrail(void) {
    for (int i = 0; i < trailCount - 1; i++) { //ενα απλο for loop ( μεχρι index-1 επειδη εδω ζωγραφισουμε ευθυγραμμα τμημτατα)
        DrawLineV(trailPoints[i], trailPoints[i + 1], RED); //Παμε στο ενα και στο αμεσος επμενο και ζωγραφιζουμε ενα ευθυγραμμο τμημα 
        DrawCircleV(trailPoints[i], 2.0f, MAROON); //Ζωγραφιζουμε το σημειο για να φγαινεται
    }
}

void DrawJumpTrajectory(Vector2 currentPos, Vector2 currentVelocity, float g, float floorY) {
    int numSegments = 30; //τα ευθυγραμμα τμηματα που την αποτελουν 
    /*
    ΕΔΩ ΕΧΟΥΜΕ ΠΟΛΥ ΛΙΓΑ ΜΑΘΗΜΑΤΙΚΑ!!!
    Ειπαμε οτι η εξισωβση της κατακόρυφης θεσης του παιχτη σε συναρτηση με τον χρονο t ειναι
    y(t) = y_0 - v_y * t +1/2 * g*t^2
    εμεις θελουμε να βρουμε την t που ο παιχτης φτανει στο εδαφος δηλαδη το y=floorY
    αρα floorY = y_0 - v_y * t +1/2 * g*t^2 => 1/2 * g*t^2 - v_y * t + (y_0-floorY)=0
    Παιρνυομε διακρυβουσα:
    α = 1/2g β = v_y γ=(y_0-floorY)
    */
    float a = 0.5f * g;
    float b = currentVelocity.y;
    float c = currentPos.y - floorY;
    float discriminant = b * b - 4.0f * a * c; // η διακρυνουθσα β^2 - 4αγ
    if (discriminant < 0) return;

    float timeRemaining = (-b + sqrtf(discriminant)) / (2.0f * a); //οι ριζα αυτη ειναι παντα θετικη γιατι (y-floorY)<0 παντα ( ειμαστε πανω απο το εδαφος) 
    float timeStep = timeRemaining / numSegments; //χωριζουμε το t σε 30 ισα ευθυγραμμα τμηματα

    Vector2 previousPoint = currentPos; //οριζουμε το προηγουμενο point με το τωρινο

    for (int i = 1; i <= numSegments; i++) { //αν ειναι λιγοτερο απο το μεγιστο αριθμος ευθυγραμμων τμηματων επαναλαβε
        float t = i * timeStep; //υπολογιζει τον λανθανων χρονο που εχει περασει απο την στιγμη της "προβλεψης" μεχρι μεχρι το i

        Vector2 currentPoint;
        currentPoint.x = currentPos.x + currentVelocity.x * t; //το χ στην χρινικη στιγμη t ( ομαλη εξισωση κινησης αφου δεν υπαρχει acc)
        currentPoint.y = currentPos.y + currentVelocity.y * t + 0.5f * g * (t * t); //ο τυπος για να βρουμε το y

        if (currentPoint.y > floorY) {
            currentPoint.y = floorY;
        }

        DrawLineV(previousPoint, currentPoint, BLUE);
        DrawCircleV(currentPoint, 2.0f, DARKBLUE);

        previousPoint = currentPoint; //οριζουμε το prev point με το τρεχων

        if (currentPoint.y >= floorY) break;
    }
}
/*ΤΕΛΟΣ ΤΟ ΠΕΡΙΕΡΓΟ ΚΟΜΜΑΤΙ*/

//initialize το jump
static inline JumpConfig InitJumpConfig(float h, float x_h, float v_x) {
    return (JumpConfig){
        .h = h,
        .x_h = x_h,
        .jump_force = -(2.0f * h * v_x) / x_h,
        .gravity = (2.0f * h * v_x * v_x) / (x_h * x_h)
    };
}


int main(void)
{
    const float player_v_x = 250.0f; // το foot speed
    float floorY = 350.0f; //το y του πατωματος
    //ο παιχτης
    Player p = {
        .v_x = player_v_x,
        .pos = (Vector2){100.0f, floorY},
        .velocity = (Vector2){0.0f, 0.0f},
        .is_grounded = true,
        .jump_conf= InitJumpConfig(200.0f, 150.0f, player_v_x), // [0] -> 1o αλμα (Ground)
        
    };

    InitWindow(700,500,"Hello");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if(IsKeyPressed(KEY_SPACE) && p.is_grounded)
        {
            p.velocity.y = p.jump_conf.jump_force;
            if(p.is_grounded){
                ClearTrail();
            }
            p.is_grounded = false;
        } 
        if(p.is_grounded){
            p.velocity.x = 0.0f;
            //αυξανουμε - μειωνουμε την ταχυτητα με το footspeed
            if(IsKeyDown(KEY_D)) p.velocity.x +=p.v_x; 
            if(IsKeyDown(KEY_A)) p.velocity.x -=p.v_x;
            //ο τυπος της x χωρις acc
            p.pos.x += p.velocity.x*dt;
        }else{//αν ειναι στον αερα τοτε
            if (IsKeyDown(KEY_D)) p.velocity.x = p.v_x;
            if (IsKeyDown(KEY_A)) p.velocity.x = -p.v_x;
            //το velocity verlet
            p.pos.x += p.velocity.x * dt;
            p.pos.y += p.velocity.y * dt + 0.5f*p.jump_conf.gravity*(dt*dt);
            p.velocity.y +=p.jump_conf.gravity*dt;
            //καλουμε για το γραφιμα
            AddTrailPoint((Vector2){ p.pos.x + 10.0f, p.pos.y + 10.0f });
            if(p.pos.y >=floorY) //αν περναει κατω απο το floor, ειναι κατι σαν collision εδω
            {
                p.pos.y = floorY; //το βαζουμε απο επανω στο floar
                p.velocity.y = 0.0f; //το vel στο y ειναι 0
                p.is_grounded = true; //ακουμαπει το πατωμα
            }
        }



        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawLine(0, (int)floorY + 20, 700, (int)floorY + 20, GRAY); //το πατωμα
            
            DrawRecordedTrail(); // ζωγραφθζω αν εχω τα προηγουμε trails
            DrawJumpTrajectory(p.pos, p.velocity,p.jump_conf.gravity, floorY);
            DrawRectangleV(p.pos, (Vector2){20, 20}, RED);
            DrawText(TextFormat("Current Gravity: %.2f", p.jump_conf.gravity), 10, 35, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
