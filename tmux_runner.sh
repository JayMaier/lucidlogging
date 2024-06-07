
# Start tmux session named 'lucid_testing'
tmux new-session -d -s lucid_testing

# Keep tmux session running
tmux attach-session -t lucid_testing

# tail -f /dev/null