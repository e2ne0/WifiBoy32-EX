from selenium import webdriver
from selenium.webdriver.support.ui import Select
from selenium.webdriver.common.keys import Keys
from bs4 import BeautifulSoup
driver = webdriver.Firefox()
driver.get("https://www.digole.com/tools/digole-emu.php")

moduleSelect = Select(driver.find_element_by_id("module"))
moduleSelect.select_by_value("8")

commandSelect = Select(driver.find_element_by_id("Command"))
commandSelect.select_by_value("7")

s = 0
for rgb in range(0,256):
    rgbInput = driver.find_element_by_id("B1C")
    rgbInput.clear()
    rgbInput.send_keys(str(s))
    rgbInput.send_keys(Keys.RETURN)
    rgba = rgbInput.value_of_css_property("background-color")
    
    print("{"+rgba[4:-1]+"},")
    s+=1
