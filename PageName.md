Yawing全称是（yet another wireless insurance-against-theft, next generation ） ,它是一个DIY无线家庭防盗器的项目。


# Introduction #

项目背景介绍：很多家庭对家庭安全非常担心，强烈需要一个家庭防盗器。

技术背景介绍：主芯片采用MSP430F2274，无线发射芯片先定在CC2500，通信协议采用SimplicTI，可以使用TI的ez430-cc2500开发套件。


# Details #

成本要求：设计中会控制成本，但不是对成本特别敏感的项目。

Step 1. 项目目标（Project Goals） 1.1 利益相关者（[stakeholders](stakeholders.md)），需要家庭防盗器的用户。 1.2 需求： 本地警报器

瓦斯\烟雾检测器

漏水检测器

找物器（比如挂在钥匙上的无线蜂鸣器）

防尾随（门内侧的按钮，按下就报警）




门入侵检测（红外）

电话远程报警

窗口入侵检测（不知道有什么方案）

语音警告（弄个喇叭喊话的那种：老实点，趴下，不许动。。。so cool  ）


视频（或照片）

邮件远程报警

短信远程报警




注意：1.按照实现难易度分成3类，每类中按照需求强烈度排序。 2.括号里的内容只是解释功能，并不局限于这些技术实现。

取自"http://zh.wikipedia.org/w/index.php?title=Yawing&variant=zh-cn"