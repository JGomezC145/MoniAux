#include "input.h"
#include <Keypad.h>
#include <driver/gpio.h>

//
// ----------  Keypad 3×4  ----------
//
constexpr byte ROWS = 3, COLS = 4;

char keys[ROWS][COLS] = {
    { '1','2','3','4' },
    { '5','6','7','8' },
    { '9','A','B','C' }
};

byte rowPins[ROWS] = { 10, 11, 12 };     // ver tabla arriba
byte colPins[COLS] = { 13, 14, 15, 16 };

Keypad kpd = Keypad( makeKeymap(keys),
                     rowPins, colPins,
                     ROWS, COLS );

//
// ----------  Encoder (A = CLK, B = DT, SW)  ----------
//
constexpr gpio_num_t ENC_A = GPIO_NUM_18;
constexpr gpio_num_t ENC_B = GPIO_NUM_17;
constexpr gpio_num_t ENC_SW= GPIO_NUM_9;

// ---- encoder ----
static volatile int32_t encSteps = 0;   //  acumulador ±1 por flanco
static int32_t lastDisp = 0;            //  leído por la tarea


static QueueHandle_t qInput;

static void IRAM_ATTR encoder_isr(void*)
{
    static const int8_t tab[16] = {0,-1,1,0, 1,0,0,-1,-1,0,0,1, 0,1,-1,0};
    static uint8_t state = 0;

    state = (state << 2) |                     // desplaza
            (gpio_get_level(ENC_A) << 1) |
             gpio_get_level(ENC_B);

    encSteps += tab[state & 0x0F];             // ±1 ó 0
}


//
// ----------  Tarea en Core 1  ----------
//
static void TaskInput(void*){
    // antirrebote botones encoder (llamado cada 5 ms)
    const TickType_t scanPeriod = pdMS_TO_TICKS(5);
    bool lastSw = HIGH;

    for(;;){
        // --- teclado ---
        char ch = kpd.getKey();          // no bloquea
        if(ch){
            InputEvent ev{ KEYPAD_CHAR, ch };
            xQueueSend(qInput,&ev,0);
        }

        // --- botón del encoder ---
        bool sw = digitalRead(ENC_SW);
        if(lastSw==HIGH && sw==LOW){     // flanco ↓
            InputEvent ev{ ENC_BTN_PRESS, 0 };
            xQueueSend(qInput,&ev,0);
        }
        lastSw = sw;

        // --- encoder detents ---
        int32_t diff = encSteps - lastDisp;        // pasos nuevos
        if ( diff <= -4 ) {                        // 4 pasos → 1 click izq
            InputEvent ev{ ENC_ROT_LEFT, 0 };
            xQueueSend(qInput, &ev, 0);
            lastDisp -= 4;                         // consume los 4
        }
        else if ( diff >= 4 ) {                    // 4 pasos → 1 click der
            InputEvent ev{ ENC_ROT_RIGHT, 0 };
            xQueueSend(qInput, &ev, 0);
            lastDisp += 4;
        }

        vTaskDelay(scanPeriod);
    }
}

void startInputTask(){
    qInput = xQueueCreate(32,sizeof(InputEvent));

    // pines encoder
    pinMode(ENC_SW, INPUT_PULLUP);
    gpio_set_direction(ENC_A, GPIO_MODE_INPUT);
    gpio_set_direction(ENC_B, GPIO_MODE_INPUT);
    gpio_set_intr_type(ENC_A, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(ENC_B, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENC_A, encoder_isr, nullptr);
    gpio_isr_handler_add(ENC_B, encoder_isr, nullptr);

    // arranca la tarea en Core 1 (prioridad 3)
    xTaskCreatePinnedToCore(TaskInput,"InputTask",
                            3072,nullptr,3,nullptr,1);
}

bool getInputEvent(InputEvent& ev, TickType_t timeout){
    return xQueueReceive(qInput,&ev,timeout)==pdTRUE;
}
