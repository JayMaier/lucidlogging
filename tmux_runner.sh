
# Start tmux session named 'lucid_testing'
tmux new-session -d -s lucid_testing

# Run your C++ executable inside tmux session
# tmux send-keys -t lucid_testing "sh ./ArenaSDK/ArenaSDK_Linux_ARM64/ArenaSDK_Linux_ARM64.conf" C-m

tmux send-keys -t lucid_testing "./jay_testing/Cpp_ptp_sync" C-m

# Keep tmux session running
tmux attach-session -t lucid_testing
