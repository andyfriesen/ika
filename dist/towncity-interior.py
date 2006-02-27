from mapsys import *

def OutsideClydesHouse():
    field.mapSwitch('towncity.ika-map', 11 * 16, 4 * 16)

def OutsideShop():
    field.mapSwitch('towncity.ika-map', 14 * 16, 9 * 16)

def Merchant():
    text(
        caption='Merchant',
        text="Looking for something?")
    Shop(
        'Aspirin',
        'Buffrin',
        'Shirt',
        'Leather Vest',
        'Wood Sword'
        )

MerchantMove = Wander(150, 16)