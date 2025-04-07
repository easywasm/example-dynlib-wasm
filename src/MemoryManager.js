// Memory management with free-list and best-fit allocation
export default class MemoryManager {
  constructor(initialOffset = 8192, env) {
    this.freeList = []; // List of {ptr, size} objects
    this.allocations = new Map(); // Track active allocations
    this.currentOffset = initialOffset;
    this.ALIGNMENT = 8;
    this.env = env;
  }

  // Align size to nearest ALIGNMENT bytes
  align(size) {
    return (size + (this.ALIGNMENT - 1)) & ~(this.ALIGNMENT - 1);
  }

  // Find best-fit block in free list
  findBestFit(size) {
    let bestFitIndex = -1;
    let bestFitSize = Infinity;

    for (let i = 0; i < this.freeList.length; i++) {
      const block = this.freeList[i];
      if (block.size >= size && block.size < bestFitSize) {
        bestFitIndex = i;
        bestFitSize = block.size;
      }
    }

    if (bestFitIndex !== -1) {
      return this.freeList.splice(bestFitIndex, 1)[0];
    }
    return null;
  }

  // Merge adjacent free blocks
  mergeFreeBlocks() {
    this.freeList.sort((a, b) => a.ptr - b.ptr);

    for (let i = 0; i < this.freeList.length - 1; i++) {
      const current = this.freeList[i];
      const next = this.freeList[i + 1];

      if (current.ptr + current.size === next.ptr) {
        // Merge blocks
        current.size += next.size;
        this.freeList.splice(i + 1, 1);
        i--; // Recheck this position
      }
    }
  }

  malloc(size) {
    const alignedSize = this.align(size);

    // Try to find a suitable free block
    const freeBlock = this.findBestFit(alignedSize);
    if (freeBlock) {
      // If the block is significantly larger, split it
      if (freeBlock.size > alignedSize + this.ALIGNMENT * 2) {
        const remainingSize = freeBlock.size - alignedSize;
        this.freeList.push({
          ptr: freeBlock.ptr + alignedSize,
          size: remainingSize,
        });
      }

      this.allocations.set(freeBlock.ptr, alignedSize);
      return freeBlock.ptr;
    }

    // If no suitable free block found, allocate from the top
    const ptr = this.currentOffset;
    this.currentOffset += alignedSize;
    this.allocations.set(ptr, alignedSize);

    // Check if we need to grow memory
    const memoryPages = this.env.memory.buffer.byteLength / 65536;
    const requiredPages = Math.ceil(this.currentOffset / 65536);
    if (requiredPages > memoryPages) {
      this.env.memory.grow(requiredPages - memoryPages);
    }

    return ptr;
  }

  free(ptr) {
    if (!this.allocations.has(ptr)) {
      console.warn("Attempting to free unallocated pointer:", ptr);
      return;
    }

    const size = this.allocations.get(ptr);
    this.allocations.delete(ptr);

    // Add to free list
    this.freeList.push({ ptr, size });

    // Merge adjacent blocks to prevent fragmentation
    this.mergeFreeBlocks();
  }

  // Debug utilities
  printMemoryMap() {
    console.log("\nMemory Map:");
    console.log("Allocated blocks:");
    for (const [ptr, size] of this.allocations) {
      console.log(`  ${ptr}: ${size} bytes`);
    }
    console.log("Free blocks:");
    for (const block of this.freeList) {
      console.log(`  ${block.ptr}: ${block.size} bytes`);
    }
    console.log(`Current offset: ${this.currentOffset}`);
  }
}
