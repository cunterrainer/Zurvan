#include "GUI.h"

#include <cstdlib>
#include <cstring> // TODO fix includes

#include "styles/dark/style_dark.h"
#include "styles/amber/style_amber.h"
#include "styles/ashes/style_ashes.h"


void FloatingWindow::Init() const noexcept
{
    GuiLoadStyleDark();
}