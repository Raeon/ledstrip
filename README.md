
# Segments
The LED strip can be divided into segments. Each segments is identified by their assigned index. These segments each have their own pattern.


# Modes
Every segment has one mode that dictates it's behaviour.

## None
Does nothing. This is the default mode. All LEDs are turned off.

## Unified
Everything has a single color.
- `color`: RGBA

## Segmented
A repeated pattern of colors. If `blend` is specified then there will be a gradient exctly `blend` pixels long between every two segments.
The `alignment` dictates at which part of the segment the pattern starts, and `direction` specifies in which direction the pattern moves, if at all. This also requires `speed` to be specified and greater than zero.
- `segments`: []
  + `color`: RGBA
  + `length`: pixels
- `blend`: pixels
- `speed`: pixels/second
- `alignment`: boolean (false is low, true is high)
- `direction`: boolean (false is low, true is high)

## Progress
A progress bar with several stages. Imagine a health bar that goes from green to yellow to red the less health you have. Each stage has it's own `color` and a percentage `from` where it starts.
In the case of the health bar above, you could have a green segment starting at 70%, yellow at 40% and red at 0%.
- `stages`: []
  + `color`: RGBA
  + `from`: percentage
- `value`: percentage
- `alignment`: boolean (false is low, true is high)
- `smooth`: boolean (false is hard, true is gradual transitions)

# Interrupts
An interruption to the regular mode. This is applied **on top of the regular mode**. It can be used to show special single-fire events. An example could be a red fade when receiving damage in a video game.

## Blink
Blinks all LEDs between mode and OFF. **This means that this interrupt will do nothing if the LEDs were already turned off.** Optionally a pixel range can be specified for this interrupt to be applied to. If none is given, it will be applied to the entire segment.
- `frequency`: toggles/second
- `duration`: millis
- `range`: {}
  + `begin`: pixel
  + `end`: pixel

## Fade
Fades from the specified `color` to the regular mode over the given `duration` milliseconds.
- `color`: RGBA
- `duration`: millis
- `range`: {}
  + `begin`: pixel
  + `end`: pixel
