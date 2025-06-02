Your task is to generate a detailed design document for `[Design Document Name]`. This document will be based on its corresponding "implementation guide" (a specific section from an external document which will be provided to you) and the overarching `[Architecture Reference Document]`.

**Core Objective:**
Produce a comprehensive and meticulous design document that will serve as a blueprint for implementing the specified system component or aspect. This document must reflect a deep understanding of the CKOS project's requirements, its existing architecture as defined in `[Architecture Reference Document]`, and best practices for embedded systems development on an STM32L4 platform.

**Key Inputs for Your Task:**
1.  **The "Implementation Guide" (Specific Section from an external document):** This is your primary input, defining the scope and required content for `[Design Document Name]`. *This specific section will be provided to you immediately following this main prompt.*
2.  **The "Architecture Reference" (`[Architecture Reference Document]`):** This is the foundational architectural document for the CKOS project. *You should treat this document as if it's readily available for your detailed reference throughout this task.* You MUST ensure your design for `[Design Document Name]` aligns with, details, and expands upon the principles, components, tasks, communication mechanisms, memory management strategies, and interactions described in `[Architecture Reference Document]`. Actively cross-reference, integrate, and justify your design decisions against this architectural reference.

**Detailed Process & Methodology for Generating the Document:**

1.  **Understand Specific Requirements:**
    *   Thoroughly analyze the provided "Implementation Guide" for `[Design Document Name]`. Identify every item, sub-item, and implied requirement.

2.  **Architectural Integration and Elaboration:**
    *   For each requirement in the "Implementation Guide," identify all relevant systems, tasks, components, data structures, and interfaces mentioned in the "Architecture Reference" (`[Architecture Reference Document]`).
    *   Explicitly detail how your proposed design for `[Design Document Name]` integrates with, utilizes, or extends these architectural elements. For instance, if designing a specific system component, explain how it interacts with other relevant tasks, specifically referencing the Inter-Task Communication (IPC) mechanisms and operational pipelines outlined in `[Architecture Reference Document]`.

3.  **In-Depth Design and Critical Thinking ("Thinking Hard"):**
    *   For each sub-section or key feature outlined in the "Implementation Guide" for `[Design Document Name]`:
        *   **Elaborate Implementation Strategies:** Describe *how* it would be implemented within the CKOS framework.
        *   **Define Data Structures:** Propose specific C `struct`s, `enum`s, or other data representations, ensuring they are efficient for an embedded system.
        *   **Define APIs:** Specify clear C function signatures, parameters (name, type, direction, purpose), return values, and any critical preconditions or postconditions.
        *   **Algorithms:** Outline key algorithms or logic flows in sufficient detail.
        *   **Address Constraints & Resources:** Explicitly consider and discuss memory usage (RAM/Flash budgets), processing load, real-time constraints, and power efficiency implications, referencing strategies from `[Architecture Reference Document]` where applicable.
        *   **Edge Cases & Error Handling:** Identify potential challenges, edge cases, race conditions, and failure modes. Propose robust handling mechanisms.
        *   **Justify Choices:** Crucially, explain *why* your proposed design choices (data structures, algorithms, APIs, etc.) are suitable and optimal for the CKOS project. Justify these against the "Architecture Reference," embedded systems best practices, and the specific requirements of the component.

4.  **Draft Document Generation:**
    *   Produce an initial, comprehensive draft of the design document.
    *   **Format and Style:** The document MUST emulate the style, structure, formatting (e.g., markdown), and level of detail found in `[Architecture Reference Document]`. Use clear hierarchical headings, concise but thorough explanations, and be meticulous.

5.  **Iterative Refinement - Proactive Q&A Phase:**
    *   After generating the draft, critically review your own work. Identify areas where:
        *   Multiple implementation approaches are viable, and a decision requires weighing specific trade-offs.
        *   Details in the source documents are underspecified for a complete design, requiring clarification.
        *   Assumptions were made during your design process that need validation from me (the user).
    *   For each such area, formulate precise questions. Structure each question as follows to facilitate informed decision-making:
        *   **Context:** Briefly describe the design aspect or component in question.
        *   **Question:** Clearly state the specific design choice, ambiguity, or assumption.
        *   **Option 1:** Describe the first potential approach or interpretation.
            *   **Pros:** (List 2-3 key advantages, e.g., performance, memory, simplicity)
            *   **Cons:** (List 2-3 key disadvantages or trade-offs)
        *   **Option 2:** Describe the second potential approach or interpretation.
            *   **Pros:**
            *   **Cons:**
        *   *(Include more options if relevant)*
        *   **Your Recommendation (Optional but Highly Encouraged):** Briefly state which option you lean towards and provide a concise justification based on your analysis of the CKOS architecture, requirements, and embedded principles.

6.  **Finalization (Post-Feedback):**
    *   Once I provide answers, decisions, or clarifications for your questions, incorporate this feedback thoroughly to refine and finalize the design document for `[Design Document Name]`.

**Deliverables for this Task:**
1.  The initial comprehensive draft of the design document for `[Design Document Name]`.
2.  A structured list of clarification questions, each with clearly defined options, pros, and cons, as detailed in step 5.

**Guiding Principle:**
Your goal is not merely to expand on bullet points. It is to perform a rigorous design exercise, leveraging `[Architecture Reference Document]` as your steadfast guide, to produce a document that is technically sound, detailed enough for subsequent implementation, and thoroughly well-reasoned. Think like an embedded systems architect responsible for designing this specific part of the CKOS firmware. Be extremely careful, deliberate, and meticulous in your thinking and writing.