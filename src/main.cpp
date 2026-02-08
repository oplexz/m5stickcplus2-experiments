#include <Arduino.h>
#include <M5GFX.h>
#include <Button.h>

static M5GFX screen;

static constexpr uint8_t kButtonUpPin = 35;     // Up
static constexpr uint8_t kButtonDownPin = 39;   // Down
// static constexpr uint8_t kButtonSelectPin = 37; // Select

static Button buttonUp(kButtonUpPin);
static Button buttonDown(kButtonDownPin);
// static Button buttonSelect(kButtonSelectPin);

static constexpr const char* kItems[] = {
    "First item",
    "Second item",
    "Third item",
    "Fourth item",
    "Fifth item",
    "Sixth item",
    "Seventh item",
    "Eighth item",
    "Ninth item",
    "Tenth item",
};

static constexpr size_t kItemCount = sizeof(kItems) / sizeof(kItems[0]);
static constexpr int kLineHeight = 20;
static constexpr int kListTop = 8;
static constexpr int kListLeft = 8;
static constexpr int kTextSize = 2;
static constexpr int kScrollPadding = 4;

static int selectedIndex = 0;
static int topIndex = 0;

static int visibleRows()
{
    return (screen.height() - kListTop - kScrollPadding) / kLineHeight;
}

static void drawList()
{
    screen.fillScreen(TFT_BLACK);
    screen.setTextSize(kTextSize);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);

    int rows = visibleRows();
    for (int row = 0; row < rows; ++row)
    {
        int itemIndex = topIndex + row;
        if (itemIndex >= static_cast<int>(kItemCount))
        {
            break;
        }

        int y = kListTop + row * kLineHeight;
        bool isSelected = (itemIndex == selectedIndex);

        if (isSelected)
        {
            screen.fillRect(0, y - 1, screen.width(), kLineHeight, TFT_DARKGREY);
            screen.setTextColor(TFT_BLACK, TFT_DARKGREY);
        }
        else
        {
            screen.setTextColor(TFT_WHITE, TFT_BLACK);
        }

        screen.setCursor(kListLeft, y);
        screen.print(kItems[itemIndex]);
    }
}

static void ensureSelectionVisible()
{
    int rows = visibleRows();
    if (selectedIndex < topIndex)
    {
        topIndex = selectedIndex;
    }
    else if (selectedIndex >= topIndex + rows)
    {
        topIndex = selectedIndex - rows + 1;
    }
}

void setup()
{
    screen.init();
    screen.setRotation(3);
    screen.setBrightness(128);

    buttonUp.begin();
    buttonDown.begin();
    // buttonSelect.begin();

    drawList();
}

void loop()
{
    bool updated = false;

    if (buttonUp.pressed())
    {
        if (selectedIndex == 0)
        {
            selectedIndex = static_cast<int>(kItemCount) - 1;
        }
        else
        {
            selectedIndex--;
        }
        updated = true;
    }

    if (buttonDown.pressed())
    {
        if (selectedIndex >= static_cast<int>(kItemCount) - 1)
        {
            selectedIndex = 0;
        }
        else
        {
            selectedIndex++;
        }
        updated = true;
    }

    if (updated)
    {
        ensureSelectionVisible();
        drawList();
    }

    delay(10);
}
