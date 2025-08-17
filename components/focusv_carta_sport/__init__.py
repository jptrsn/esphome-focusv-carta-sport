import os

# Define the component's root path
COMPONENT_PATH = os.path.dirname(os.path.abspath(__file__))

# Import necessary files and classes
from .focusv_carta_sport import FocusVCartaSportComponent
from .const import SERVICE_UUID, CHAR_NOTIFY_UUID

# Make the necessary components available for use during the build process
cg.add_component(FocusVCartaSportComponent)