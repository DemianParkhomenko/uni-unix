#! /bin/bash
echo "Dear $LOGNAME, you are welcome❣️"
echo "Date: $(date -d "today" +"%A, %B %d, %Y")"
echo "Time:$(date -d "today" +"%r")"
echo "Hint: EEST means Eastern European Summer Time UTC+3👀"

hours=$(date +"%H")
part_of_day=""
if [ $hours -gt 6 ] && [ $hours -le 12 ]; then
  part_of_day="morning"
elif [ $hours -gt 12 ] && [ $hours -le 16 ]; then
  part_of_day="afternoon"
elif [ $hours -gt 16 ] && [ $hours -le 20 ]; then
  part_of_day="evening"
else
  part_of_day="night"
fi
echo "Part of day: $part_of_day"
grep "$LOGNAME" "/etc/passwd" | cut -d ":" -f 1- --output-delimiter=$'\n'
