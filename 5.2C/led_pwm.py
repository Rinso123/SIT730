import sys
import tkinter as tk
from gpiozero import PWMLED


class LEDControllerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("LED Control")
        self.root.geometry("300x380")

        # PWM LEDs
        self.red_led = PWMLED(18)
        self.green_led = PWMLED(23)
        self.blue_led = PWMLED(24)

        # Cycling states
        self.cycling = False
        self.cycle_duty = 0.0
        self.cycle_direction = 0.02

        self.setup_ui()
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def setup_ui(self):
        # Sliders
        self.red_slider = self.create_slider(
            self.root, "Red (GPIO 18)", self.update_red
        )
        self.green_slider = self.create_slider(
            self.root, "Green (GPIO 23)", self.update_green
        )
        self.blue_slider = self.create_slider(
            self.root, "Blue (GPIO 24)", self.update_blue
        )

        # Controls
        self.timer_btn = tk.Button(
            self.root,
            text="Enable Auto-Cycle",
            command=self.toggle_timer_mode,
        )
        self.timer_btn.pack(pady=15, padx=30, fill="x")

        exit_btn = tk.Button(self.root, text="Exit", command=self.on_close)
        exit_btn.pack(pady=5, padx=30, fill="x")

    def create_slider(self, parent, label_text, command_cb):
        lbl = tk.Label(parent, text=label_text)
        lbl.pack(anchor="w", padx=30, pady=(10, 0))

        slider = tk.Scale(
            parent,
            from_=0,
            to=100,
            orient="horizontal",
            command=command_cb,
        )
        slider.pack(fill="x", padx=30)
        return slider

    def update_red(self, val):
        if not self.cycling:
            self.red_led.value = float(val) / 100.0

    def update_green(self, val):
        if not self.cycling:
            self.green_led.value = float(val) / 100.0

    def update_blue(self, val):
        if not self.cycling:
            self.blue_led.value = float(val) / 100.0

    def toggle_timer_mode(self):
        if self.cycling:
            self.cycling = False
            self.timer_btn.config(text="Enable Auto-Cycle")
        else:
            self.cycling = True
            self.timer_btn.config(text="Stop Auto-Cycle")
            self.run_timer_loop()

    def run_timer_loop(self):
        if not self.cycling:
            return

        self.cycle_duty += self.cycle_direction
        if self.cycle_duty >= 1.0:
            self.cycle_duty = 1.0
            self.cycle_direction = -0.02
        elif self.cycle_duty <= 0.0:
            self.cycle_duty = 0.0
            self.cycle_direction = 0.02

        green_intensity = self.cycle_duty
        blue_intensity = 1.0 - self.cycle_duty

        self.green_led.value = green_intensity
        self.blue_led.value = blue_intensity

        self.green_slider.set(int(green_intensity * 100))
        self.blue_slider.set(int(blue_intensity * 100))

        self.root.after(30, self.run_timer_loop)

    def on_close(self):
        self.red_led.close()
        self.green_led.close()
        self.blue_led.close()
        self.root.destroy()
        sys.exit()


if __name__ == "__main__":
    root = tk.Tk()
    app = LEDControllerApp(root)
    root.mainloop()
