import { navbar } from "vuepress-theme-hope";

export const zhNavbar = navbar([
  "/",
  {
    text: "快速入门",
    icon: "lightbulb",
    prefix: "/get-started/",
    children: [
      "huangshan/README.md",
    ],
  },
]);
